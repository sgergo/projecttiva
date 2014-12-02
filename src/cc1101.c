#include <stdint.h>
#include "cc1101.h"
#include "cc1101regs.h"

cc1101_interface_t cc1101_interface;

void cc1101_write_reg(uint8_t reg, uint8_t val) {
	cc1101_interface.interface_spi_low();
	cc1101_interface.interface_spi_send(reg);
	cc1101_interface.interface_spi_send(val);
	cc1101_interface.interface_spi_high();
}

uint8_t cc1101_read_reg(uint8_t reg) {
	uint8_t retval;
	cc1101_interface.interface_spi_low();
	cc1101_interface.interface_spi_send(reg | CC1101_READ_SINGLE);
	retval = cc1101_interface.interface_spi_send(0xff);
	cc1101_interface.interface_spi_high();
	return retval;
}

uint8_t cc1101_read_status_reg(uint8_t reg) {
	uint8_t retval;
	cc1101_interface.interface_spi_low();
	cc1101_interface.interface_spi_send(reg | CC1101_READ_BURST);
	retval = cc1101_interface.interface_spi_send(0xff);
	cc1101_interface.interface_spi_high();
	return retval;
}

uint8_t cc1101_cmd(uint8_t cmd) {
	uint8_t retval;
	cc1101_interface.interface_spi_low();
	retval = cc1101_interface.interface_spi_send(cmd);
	cc1101_interface.interface_spi_high();
	return retval;
}