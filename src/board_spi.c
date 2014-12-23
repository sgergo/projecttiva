#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "board_spi.h"
#include "boardconfig.h"

//#define USE_FSS_PIN

static void board_spi_cspin_init(void) {
	ROM_SysCtlPeripheralEnable(SPICSPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(SPICSPINPERIPHERIALBASE, SPICSPIN);
	ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, SPICSPIN); // Pull CS high
}

void board_spi_cspin_low(void) {
	ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, 0);
}

void board_spi_cspin_high(void) {
    ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, SPICSPIN);
}

uint32_t board_spi_sendbyte(uint8_t bytetosend) {
	uint32_t rxbuf;
#ifdef CUSTOM_CS_PIN
	board_spi_cspin_low();
#endif
	ROM_SSIDataPut(SPIPERIPHERIALBASE, (uint32_t) bytetosend);
	while(ROM_SSIBusy(SPIPERIPHERIALBASE))
		;
    ROM_SSIDataGet(SPIPERIPHERIALBASE, &rxbuf);
#ifdef CUSTOM_CS_PIN
	board_spi_cspin_high();
#endif
    return rxbuf;
}

uint32_t board_spi_write(uint8_t bytetosend) {
    uint32_t rxbuf;

    ROM_SSIDataPut(SPIPERIPHERIALBASE, (uint32_t) bytetosend);
    while(ROM_SSIBusy(SPIPERIPHERIALBASE))
        ;
    ROM_SSIDataGet(SPIPERIPHERIALBASE, &rxbuf);

    return rxbuf;
}

void board_spi_init(void) {
    uint32_t rxbuf;

    ROM_SysCtlPeripheralEnable(SPIPERIPHERIAL);
    ROM_SysCtlPeripheralEnable(SPIPINPERIPHERIAL);

    ROM_GPIOPinConfigure(SPISCLKPINTYPE);
    ROM_GPIOPinConfigure(SPISOMIPINTYPE);
    ROM_GPIOPinConfigure(SPISIMOPINTYPE);
   
#ifdef USE_FSS_PIN 
    ROM_GPIOPinConfigure(SPISFSSPINTYPE);
    ROM_GPIOPinTypeSSI(SPIPINPERIPHERIALBASE, SPISIMOPIN | SPISOMIPIN |  SPISFSSPIN | 
                   SPISCLKPIN);
#else
    // We don't use hardware CS here (SFSS), we init our custom CS pin(s) instead
    board_spi_cspin_init();
    ROM_GPIOPinTypeSSI(SPIPINPERIPHERIALBASE, SPISIMOPIN | SPISOMIPIN | 
                   SPISCLKPIN);   
#endif

    ROM_SSIConfigSetExpClk(SPIPERIPHERIALBASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 2000000, 8);
    ROM_SSIEnable(SPIPERIPHERIALBASE);

    // Clear RX fifo
    while(ROM_SSIDataGetNonBlocking(SPIPERIPHERIALBASE, &rxbuf))
        ;
}

spi_if_entry_t spi_if[] = {
    {board_spi_cspin_low, board_spi_cspin_high, board_spi_write, board_spi_sendbyte},
    {NULL, NULL, NULL, NULL}
};
