#ifndef CC1101_H
#define CC1101_H

typedef struct {
	void (*spi_high) (void);
	void (*spi_low) (void);
	uint8_t (*spi_write) (uint8_t bytetosend);
	void (*board_delay_us) (uint32_t delay);
	void (*board_delay_ms) (uint32_t delay);
}cc1101_interface_t;

typedef struct {
	uint32_t lofasz:1;
}cc1101_switch_t;

typedef struct {
	uint8_t device_address;
	uint8_t destination_address;
	uint8_t output_power;
	
	cc1101_switch_t switches;
}cc1101_state_t;

void cc1101_write_reg(uint8_t reg, uint8_t val);
uint8_t cc1101_read_reg(uint8_t reg);
uint8_t cc1101_read_status_reg(uint8_t reg);
uint8_t cc1101_cmd(uint8_t cmd);

void cc1101_init(void);
void cc1101_update_state(void);

#endif