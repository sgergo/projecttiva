#ifndef CC1101_H
#define CC1101_H

typedef struct {

	void (*interface_spi_high) (void);
	void (*interface_spi_low) (void);
	uint8_t (*interface_spi_send) (uint8_t bytetosend);
}cc1101_interface_t;

void cc1101_write_reg(uint8_t reg, uint8_t val);
uint8_t cc1101_read_reg(uint8_t reg);
uint8_t cc1101_read_status_reg(uint8_t reg);
uint8_t cc1101_cmd(uint8_t cmd);

#endif