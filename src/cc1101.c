
#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include "cc1101.h"
#include "cc1101regs.h"
#include "boardconfig.h"

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "board.h"
#include "console.h"

cc1101_interface_t cc1101_interface;
cc1101_state_t volatile cc1101_state;
cc1101_rxpacket_t cc1101_receivedpacket;
cc1101_txpacket_t cc1101_sentpacket;

void cc1101_write_reg(uint8_t reg, uint8_t val) {
	cc1101_interface.spi_low();
	cc1101_interface.spi_write(reg);
	cc1101_interface.spi_write(val);
	cc1101_interface.spi_high();
}

static uint8_t cc1101_write_txfifo(uint8_t *packet, uint8_t from, packetsize_t packetsize) {
	uint8_t i = 0;
	cc1101_interface.spi_low();
	cc1101_interface.spi_write(CC1101_TXFIFO | CC1101_WRITE_BURST);
	while (packetsize--) {
		cc1101_interface.spi_write(packet[from + i++]);
	} 
	cc1101_interface.spi_high();

	return cc1101_read_status_reg(CC1101_TXBYTES);
}

uint8_t cc1101_read_reg(uint8_t reg) {
	uint8_t retval;
	cc1101_interface.spi_low();
	cc1101_interface.spi_write(reg | CC1101_READ_SINGLE);
	retval = cc1101_interface.spi_write(0xff);
	cc1101_interface.spi_high();
	return retval;
}

uint8_t cc1101_read_status_reg(uint8_t reg) {
	uint8_t retval;
	cc1101_interface.spi_low();
	cc1101_interface.spi_write(reg | CC1101_READ_BURST);
	retval = cc1101_interface.spi_write(0xff);
	cc1101_interface.spi_high();
	return retval;
}

static void cc1101_read_rxfifo(uint8_t *dstpacket, uint8_t *packetsize, uint8_t *rssi_raw, uint8_t *lqi) {
	uint8_t receivedpacketsize;
	uint8_t i;

	receivedpacketsize = cc1101_read_status_reg(CC1101_RXBYTES);
	*packetsize = receivedpacketsize;
	//console_printtext("Fifo: %d \n", receivedpacketsize);
	cc1101_interface.spi_low();
	cc1101_interface.spi_write(CC1101_RXFIFO | CC1101_READ_BURST);

	for (i = 0; i < receivedpacketsize; i++) {

		if (i < (receivedpacketsize - 2)) { 
			*dstpacket = cc1101_interface.spi_write(0xff);
			dstpacket++;
			continue;
		} 

		if (i == (receivedpacketsize - 2)) { 
			*rssi_raw = cc1101_interface.spi_write(0xff);
			continue;
		}

		if (i == (receivedpacketsize - 1)) {
			*lqi = cc1101_interface.spi_write(0xff);
			continue;
		}		
	}
	cc1101_interface.spi_high();
}

uint8_t cc1101_cmd(uint8_t cmd) {
	uint8_t retval;
	cc1101_interface.spi_low();
	retval = cc1101_interface.spi_write(cmd);
	cc1101_interface.spi_high();
	return retval;
}

int8_t cc1101_calculate_rssi_dBm(uint8_t rssi_raw) {
    int8_t crssi;
    
    if (rssi_raw >= 128) {
        crssi = rssi_raw - 256;
        crssi /= 2;
        crssi -= 74;
    } else {
        crssi = rssi_raw/2;
        crssi -= 74;
    }

    return (int8_t) crssi;
}

static void cc1101_reset(void) {
	
    cc1101_interface.spi_high();
    cc1101_interface.board_delay_us(30);
    cc1101_interface.spi_low();
    cc1101_interface.board_delay_us(30);
    cc1101_interface.spi_high();
    cc1101_interface.board_delay_us(45);
    cc1101_cmd(CC1101_SRES);
    cc1101_init_regs(); // Macro with all the initial register values, see cc1101regs.h
}

void cc1101_update_state(void) {
	static cc1101_state_t cc1101_state_old;

	// Update device address if necessary
	if (cc1101_state.device_address != cc1101_state_old.device_address) {
		cc1101_write_reg(CC1101_ADDR, cc1101_state.device_address);
		// board_toggle_led(BLUE);
	}

	// Update output power if necessary
	if (cc1101_state.output_power != cc1101_state_old.output_power) {
		cc1101_write_reg(CC1101_PATABLE, cc1101_state.output_power);
		// board_toggle_led(RED);
	}

	// Update what should happen after receiving a packet
	if (cc1101_state.switches.stayinrx != cc1101_state_old.switches.stayinrx) {
		if (cc1101_state.switches.stayinrx)
			cc1101_write_reg(CC1101_MCSM1, cc1101_read_reg(CC1101_MCSM1) | 0x0c);
		else 
			cc1101_write_reg(CC1101_MCSM1, cc1101_read_reg(CC1101_MCSM1) & ~(0x0c));
		// board_toggle_led(RED);
	}

	// Update whether we want appended status bytes or not
	if (cc1101_state.switches.appendstatus != cc1101_state_old.switches.appendstatus) {
		if (cc1101_state.switches.appendstatus)
			cc1101_write_reg(CC1101_PKTCTRL1, cc1101_read_reg(CC1101_PKTCTRL1) | 0x04);
		else 
			cc1101_write_reg(CC1101_PKTCTRL1, cc1101_read_reg(CC1101_PKTCTRL1) & ~(0x04));
		// board_toggle_led(RED);
	}

	// Update whether we want crc check 
	if (cc1101_state.switches.crcenabled != cc1101_state_old.switches.crcenabled) {
		if (cc1101_state.switches.crcenabled)
			cc1101_write_reg(CC1101_PKTCTRL0, cc1101_read_reg(CC1101_PKTCTRL0) | 0x04);
		else 
			cc1101_write_reg(CC1101_PKTCTRL0, cc1101_read_reg(CC1101_PKTCTRL0) & ~(0x04));
		// board_toggle_led(RED);
	}

	cc1101_state_old = cc1101_state;	
}

void cc1101_sendpacket(uint8_t *packet, uint8_t packetsize, uint8_t waitforclearchannel) {
	uint8_t previouspacketsize;
	int32_t quotient, rem;
	uint8_t from;
	uint8_t byteswrittentofifo, status2;

	if(cc1101_state.radio_state != RADIO_STATE_IDLE) 
		return;

	cc1101_cmd(CC1101_SIDLE);
	cc1101_cmd(CC1101_SFTX);

	// Store previous and update packet length register 
	previouspacketsize = cc1101_read_reg(CC1101_PKTLEN);
	cc1101_write_reg(CC1101_PKTLEN, packetsize);

	cc1101_state.radio_state = RADIO_STATE_TXSYNCWORD;

	if (packetsize < CC1101_FIFO_SIZE) {
		cc1101_write_reg(CC1101_IOCFG2, 0x2e); // We don't need GDO2 irq, let's put it into high impedance state
		byteswrittentofifo = cc1101_write_txfifo(packet, 0, packetsize);
	
		if ( byteswrittentofifo == packetsize) {
			cc1101_cmd(CC1101_STX);
			while (cc1101_state.radio_state != RADIO_STATE_TXFINISHED)
				;
			cc1101_state.radio_state = RADIO_STATE_IDLE;
		} else {
			//TODO: tx state error
		}

	} else {

		// TX with FIFO refill necessary
		// Calculate the number of full TX FIFO chunks and the remainder
		quotient = (int32_t)packetsize / CC1101_FIFO_SIZE;
		rem = (int32_t)packetsize % CC1101_FIFO_SIZE;
		from = 0;

		// Set GDIO2 interrupt to handle FIFO refills
		cc1101_write_reg(CC1101_IOCFG2, 0x03); // Asserts when TXFIFO full, deasserts when < FIFO threshold
		cc1101_write_reg(CC1101_FIFOTHR, cc1101_read_reg(CC1101_FIFOTHR) | 0x07);

		//TODO: what should be with the GDO0 ISR?
		while (!quotient) {
			cc1101_write_txfifo(packet, from, CC1101_FIFO_SIZE);
			while (cc1101_state.fifo_state != FIFO_STATE_TX_FULL_AS)
				;
			cc1101_cmd(CC1101_STX);
			while (cc1101_state.fifo_state != FIFO_STATE_TX_FULL_DE)
				;

			from += CC1101_FIFO_SIZE;
			quotient--;
		}
		cc1101_write_reg(CC1101_IOCFG2, 0x2e); // We don't need GDO2 irq, let's put it into high impedance state
		byteswrittentofifo = cc1101_write_txfifo(packet, 0, packetsize);
		if ( byteswrittentofifo == packetsize) {
			cc1101_cmd(CC1101_STX);
			while (cc1101_state.radio_state != RADIO_STATE_TXFINISHED)
				;
			cc1101_state.radio_state = RADIO_STATE_IDLE;
		} else {
			//TODO: tx state error
		}
	}

	cc1101_write_reg(CC1101_PKTLEN, previouspacketsize);
	return;
}

void cc1101_receivepacket(uint8_t *dstpacket, uint8_t *packetsize, int8_t *rssi_dBm, uint8_t *lqi) {
	uint8_t rssi_raw;
	static uint8_t previouspacketsize;

	switch (cc1101_state.radio_state) {

		case RADIO_STATE_IDLE:
			previouspacketsize = cc1101_read_reg(CC1101_PKTLEN);
			cc1101_write_reg(CC1101_PKTLEN, cc1101_receivedpacket.expectedpacketsize);
			cc1101_receivedpacket.rcvpacketsize = 0;
			cc1101_state.radio_state = RADIO_STATE_RXSYNCWORD;
			cc1101_cmd(CC1101_SFRX);
			cc1101_cmd(CC1101_SRX);
			break; 
		case RADIO_STATE_RXFINISHED:
			cc1101_read_rxfifo(dstpacket, packetsize, &rssi_raw, lqi);
			*rssi_dBm = cc1101_calculate_rssi_dBm(rssi_raw);
			//console_printtext("Packetsize: %d \n", *packetsize);
			if (cc1101_state.switches.stayinrx) {
				cc1101_state.radio_state = RADIO_STATE_RXSYNCWORD;
			} else {
				cc1101_state.radio_state = RADIO_STATE_IDLE;
				cc1101_write_reg(CC1101_PKTLEN, previouspacketsize);
			}
			break;
		case RADIO_STATE_RXSYNCWORD:
		case RADIO_STATE_RX:
			break;

		default:
			break;
	}

	return;
}

void cc1101_GDIO0_ISR(void) {
	//board_blink_led(BLUE);
	// console_printtext("utana: %d \n", cc1101_read_status_reg(CC1101_TXBYTES));
	switch (cc1101_state.radio_state) {

		case RADIO_STATE_TXSYNCWORD:
			cc1101_state.radio_state = RADIO_STATE_TX;
			break;
		case RADIO_STATE_TX:
			cc1101_state.radio_state = RADIO_STATE_TXFINISHED;
			board_blink_led(BLUE);
			break;
		case RADIO_STATE_RXSYNCWORD:
			cc1101_state.radio_state = RADIO_STATE_RX;
			break;
		case RADIO_STATE_RX:
			cc1101_state.radio_state = RADIO_STATE_RXFINISHED;
			break;
		default:
			break;
	} 
	
	cc1101_interface.gdio0_int_clear();
}

void cc1101_GDIO2_ISR(void) {
	switch (cc1101_state.fifo_state) {
		case FIFO_STATE_ASSERT:
			break;
		case FIFO_STATE_DEASSERT:
			break;
		default:
			break;
	}
	cc1101_interface.gdio2_int_clear();
}

void cc1101_init(void) {
	cc1101_reset();
	cc1101_init_regs(); // Macro with all the initial register values, see cc1101regs.h
	cc1101_state.radio_state = RADIO_STATE_IDLE;
	cc1101_state.switches.appendstatus = 1;
	cc1101_state.switches.stayinrx = 1;
	cc1101_state.switches.crcenabled = 1;
	cc1101_update_state();
}
