#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "board.h"
#include "boardconfig.h"
#include "console_uart.h"
#include "board_spi.h"
#include "cc1101.h"

extern cc1101_interface_t cc1101_interface;
uint32_t delayloopspermicrosecond;
uint32_t delayloopspermillisecond;

static void board_configure_led(void) {
	ROM_SysCtlPeripheralEnable(LED_ALL_PINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(LED_ALL_PORTBASE, LED_RED|LED_BLUE|LED_GREEN);
	ROM_GPIOPinWrite(LED_ALL_PORTBASE, LED_RED|LED_GREEN|LED_BLUE, 0); // Switch off all LEDs
}

static void board_delay_us (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermicrosecond * delay);
}

static void board_delay_ms (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermillisecond * delay);
}

void board_init(void) {
	uint32_t clockfreq;
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
	// ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
 //                       SYSCTL_OSC_MAIN);
	clockfreq = ROM_SysCtlClockGet();
	if (clockfreq > 3E6) {
		delayloopspermicrosecond = (ROM_SysCtlClockGet() / (uint32_t)1e6) / 3;
		delayloopspermillisecond = (ROM_SysCtlClockGet() / (uint32_t)1e3) / 3;
	} else {
		//TODO: assert
	}

	board_configure_led();
	console_uart_init(DEFAULT_BAUDRATE);
	board_spi_init();

	cc1101_interface.spi_low = board_spi_cspin_low;
	cc1101_interface.spi_write = board_spi_write;
	cc1101_interface.spi_high = board_spi_cspin_high;
	cc1101_interface.board_delay_us = board_delay_us;
	cc1101_interface.board_delay_ms = board_delay_ms;	
	cc1101_init(); 
}

void board_toggle_led(led_t led) {
	uint32_t port;

	switch (led) {
		case RED:
			port = ROM_GPIOPinRead(LED_RED_PORTBASE, LED_RED);
			port ^=LED_RED;
			ROM_GPIOPinWrite(LED_RED_PORTBASE, LED_RED, port);
			break;

		case BLUE:
			port = ROM_GPIOPinRead(LED_BLUE_PORTBASE, LED_BLUE);
			port ^=LED_BLUE;
			ROM_GPIOPinWrite(LED_BLUE_PORTBASE, LED_BLUE, port);
			break;

		case GREEN:
			port = ROM_GPIOPinRead(LED_GREEN_PORTBASE, LED_GREEN);
			port ^=LED_GREEN;
			ROM_GPIOPinWrite(LED_GREEN_PORTBASE, LED_GREEN, port);
			break;
		default:
			break;
	}
}

