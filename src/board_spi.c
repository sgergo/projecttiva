#include <stdbool.h>
#include <stdint.h>
#include "board_spi.h"
#include "boardconfig.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

#ifdef CUSTOM_CS_PIN
static void board_spi_cspin_init(void) {
	ROM_SysCtlPeripheralEnable(SPICSPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(SPICSPINPERIPHERIALBASE, SPICSPIN);
	ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, SPICSPIN); // Pull CS high
}

void board_spi_cspin_high(void) {
	ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, SPICSPIN);
}

void board_spi_cspin_low(void) {
	ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, 0);
}
#endif

void board_spi_init(void) {
	uint32_t rxbuf;

	ROM_SysCtlPeripheralEnable(SPIPERIPHERIAL);
	ROM_SysCtlPeripheralEnable(SPIPINPERIPHERIAL);

	ROM_GPIOPinConfigure(SPISCLKPINTYPE);
    ROM_GPIOPinConfigure(SPISOMIPINTYPE);
    ROM_GPIOPinConfigure(SPISIMOPINTYPE);

    
#ifdef CUSTOM_CS_PIN 
    // We don't use hardware CS here (SFSS), we init our custom CS pin instead
    board_spi_cspin_init();

    ROM_GPIOPinTypeSSI(SPIPINPERIPHERIALBASE, SPISIMOPIN | SPISOMIPIN | 
                   SPISCLKPIN);
#else 
    ROM_GPIOPinConfigure(SPISFSSPINTYPE);
    ROM_GPIOPinTypeSSI(SPIPINPERIPHERIALBASE, SPISIMOPIN | SPISOMIPIN |  SPISFSSPIN | 
                   SPISCLKPIN);
#endif

    ROM_SSIConfigSetExpClk(SPIPERIPHERIALBASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);

    ROM_SSIEnable(SPIPERIPHERIALBASE);

    // Clear RX fifo
    while(ROM_SSIDataGetNonBlocking(SPIPERIPHERIALBASE, &rxbuf))
    	;
}

uint8_t board_spi_sendbyte(uint8_t bytetosend) {
	uint32_t rxbuf;
#ifdef CUSTOM_CS_PIN
	board_spi_cspin_low();
#endif
	ROM_SSIDataPut(SPIPERIPHERIALBASE, bytetosend);
	while(ROM_SSIBusy(SPIPERIPHERIALBASE))
		;
    ROM_SSIDataGet(SPIPERIPHERIALBASE, &rxbuf);
#ifdef CUSTOM_CS_PIN
	board_spi_cspin_high();
#endif
    return rxbuf;
}

uint8_t board_spi_write(uint8_t bytetosend) {
    uint32_t rxbuf;

    ROM_SSIDataPut(SPIPERIPHERIALBASE, bytetosend);
    while(ROM_SSIBusy(SPIPERIPHERIALBASE))
        ;
    ROM_SSIDataGet(SPIPERIPHERIALBASE, &rxbuf);

    return rxbuf;
}