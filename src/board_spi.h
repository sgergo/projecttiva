#ifndef BOARD_SPI_H
#define BOARD_SPI_H

#define CUSTOM_CS_PIN

void board_spi_init(void);
uint32_t board_spi_sendbyte(uint8_t bytetosend);
uint32_t board_spi_write(uint8_t bytetosend);

#ifdef CUSTOM_CS_PIN
extern void board_spi_cspin_high(void);
extern void board_spi_cspin_low(void);
extern void board_spi_cspin_toggle(void);
#endif


#endif