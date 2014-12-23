#ifndef BOARD_SPI_H
#define BOARD_SPI_H

typedef struct {
	void (*sslow) (void);
	void (*sshigh) (void);
	uint32_t (*spi_write) (uint8_t bytetosend);
	uint32_t (*spi_sendbyte) (uint8_t bytetosend);
} spi_if_entry_t;

void board_spi_init(void);
uint32_t board_spi_sendbyte(uint8_t bytetosend);
uint32_t board_spi_write(uint8_t bytetosend);


// extern void board_spi_cspin_high(void);
// extern void board_spi_cspin_low(void);
// extern void board_spi_cspin_toggle(void);


#endif
