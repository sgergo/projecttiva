
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

void cc1101_write_reg(uint8_t reg, uint8_t val) {
	cc1101_interface.spi_low();
	cc1101_interface.spi_write(reg);
	cc1101_interface.spi_write(val);
	cc1101_interface.spi_high();
}

static uint8_t cc1101_write_txfifo(uint8_t *packet, uint8_t from, packetsize_t packetsize) {
	uint32_t i = 0;
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

uint8_t cc1101_cmd(uint8_t cmd) {
	uint8_t retval;
	cc1101_interface.spi_low();
	retval = cc1101_interface.spi_write(cmd);
	cc1101_interface.spi_high();
	return retval;
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

void cc1101_init(void) {
	cc1101_reset();
	cc1101_init_regs(); // Macro with all the initial register values, see cc1101regs.h
}

void cc1101_update_state(void) {
	static cc1101_state_t cc1101_state_old;

	// Update device address if necessary
	if (cc1101_state.device_address != cc1101_state_old.device_address) {
		cc1101_write_reg(CC1101_ADDR, cc1101_state.device_address);
		board_toggle_led(BLUE);
	}

	// Update output power if necessary
	if (cc1101_state.output_power != cc1101_state_old.output_power) {
		cc1101_write_reg(CC1101_PATABLE, cc1101_state.output_power);
		board_toggle_led(RED);
	}

	cc1101_state_old = cc1101_state;	
}

void cc1101_sendpacket(uint8_t *packet, packetsize_t packetsize, flag_t waitforclearchannel) {
	uint8_t previouspacketlength;
	int32_t quotient, rem;
	uint8_t from;
	uint8_t status1, status2;
	
	// Change to IDLE state
	cc1101_cmd(CC1101_SIDLE);
	cc1101_cmd(CC1101_SFTX);
	// Store previous and update packet length register 
	previouspacketlength = cc1101_read_reg(CC1101_PKTLEN);
	cc1101_write_reg(CC1101_PKTLEN, packetsize);
	// console_printtext("pktlen: %d ", cc1101_read_reg(CC1101_PKTLEN));

	console_printtext("elotte: %d ", cc1101_read_status_reg(CC1101_TXBYTES));
	if (packetsize < CC1101_FIFO_SIZE) {
		status1 = cc1101_write_txfifo(packet, 0, packetsize);
		console_printtext("feltoltes: %d  ", status1);
		if ( status1 == packetsize)
			cc1101_cmd(CC1101_STX);
			// board_toggle_led(RED);
			// console_printtext("utana: %d\n", cc1101_read_status_reg(CC1101_TXBYTES));
	} else {
		// TX with FIFO refill necessary
		// Calculate the number full TX FIFO chunks and the remainder
		quotient = (int32_t)255 / packetsize;
		rem = 255 % packetsize;
		from = 0;
		// Set GDIO2 interrupt to handle FIFO refills
		cc1101_write_reg(CC1101_IOCFG2, 0x03); // Asserts when TXFIFO full, deasserts when < FIFO threshold
		while (quotient) {
			cc1101_write_txfifo(packet, from, packetsize);
		}
	}
}

void cc1101_GDIO0_ISR(void) {
	board_blink_led(BLUE);
	console_printtext("utana: %d \n", cc1101_read_status_reg(CC1101_TXBYTES));
	cc1101_interface.gdio0_int_clear();
}

void cc1101_GDIO2_ISR(void) {
	//board_toggle_led(GREEN);
	cc1101_interface.gdio2_int_clear();
}