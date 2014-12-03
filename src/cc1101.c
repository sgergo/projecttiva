#include <stdint.h>
#include "cc1101.h"
#include "cc1101regs.h"

#include "board.h"

cc1101_interface_t cc1101_interface;
cc1101_state_t volatile cc1101_state;

void cc1101_write_reg(uint8_t reg, uint8_t val) {
	cc1101_interface.spi_low();
	cc1101_interface.spi_write(reg);
	cc1101_interface.spi_write(val);
	cc1101_interface.spi_high();
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
}

void cc1101_init(void) {
	cc1101_reset();
	// TODO:
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