#include <stdint.h>
#include <stdbool.h>

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
//#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "boardconfig.h"
#include "board.h"
#include "board_spi.h"
#include "board_systick.h"
#include "console.h"
#include "cc1101.h"

extern cc1101_interface_t cc1101_interface;
uint32_t delayloopspermicrosecond;
uint32_t delayloopspermillisecond;

static void board_configure_led(void) {
	ROM_SysCtlPeripheralEnable(LED_ALL_PINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(LED_ALL_PORTBASE, LED_RED|LED_BLUE|LED_GREEN);
	ROM_GPIOPinWrite(LED_ALL_PORTBASE, LED_RED|LED_GREEN|LED_BLUE, 0); // Switch off all LEDs
}

void board_gdio0_edge_select(uint32_t edge) {
	uint32_t edgetype;

	switch (edge) {
	 case EDGE_SELECT_RISING:
	 	edgetype = GPIO_RISING_EDGE;
	 	break;
	 case EDGE_SELECT_FALLING:
	 	edgetype = GPIO_FALLING_EDGE;
	 	break;
 	 case EDGE_SELECT_BOTH:
	 	edgetype = GPIO_BOTH_EDGES;
	 	break;
	 default:
	 	edgetype = GPIO_RISING_EDGE;
		break;
	}

	ROM_GPIOIntTypeSet(GDO0PINPERIPHERIALBASE, GDO0PIN,	edgetype);
}

void board_gdio2_edge_select(uint32_t edge) {
	uint32_t edgetype;

	switch (edge) {
	 case EDGE_SELECT_RISING:
	 	edgetype = GPIO_RISING_EDGE;
	 	break;
	 case EDGE_SELECT_FALLING:
	 	edgetype = GPIO_FALLING_EDGE;
	 	break;
 	 case EDGE_SELECT_BOTH:
	 	edgetype = GPIO_BOTH_EDGES;
	 	break;
	 default:
	 	edgetype = GPIO_RISING_EDGE;
		break;
	}

	ROM_GPIOIntTypeSet(GDO2PINPERIPHERIALBASE, GDO2PIN,	edgetype);
}

static void board_init_gdiox_pins(void) {

	ROM_SysCtlPeripheralEnable(GDO0PINPERIPHERIAL);
	ROM_SysCtlPeripheralEnable(GDO2PINPERIPHERIAL);

    ROM_GPIOPinTypeGPIOInput(GDO0PINPERIPHERIALBASE, GDO0PIN);
    ROM_GPIOPinTypeGPIOInput(GDO2PINPERIPHERIALBASE, GDO2PIN);

    ROM_GPIOPadConfigSet(GDO0PINPERIPHERIALBASE, GDO0PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GDO2PINPERIPHERIALBASE, GDO2PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntDisable(GDO0PINPERIPHERIALBASE, GDO0PIN);
    GPIOIntDisable(GDO2PINPERIPHERIALBASE, GDO2PIN);

    GPIOIntClear(GDO0PINPERIPHERIALBASE, GDO0PIN);
    GPIOIntClear(GDO2PINPERIPHERIALBASE, GDO2PIN);

    
    GPIOIntRegister(GDO0PINPERIPHERIALBASE, cc1101_GDIO0_ISR);
    GPIOIntRegister(GDO2PINPERIPHERIALBASE, cc1101_GDIO2_ISR);

    board_gdio0_edge_select(EDGE_SELECT_RISING);
	board_gdio2_edge_select(EDGE_SELECT_RISING);

    GPIOIntEnable(GDO0PINPERIPHERIALBASE, GDO0PIN);
    GPIOIntEnable(GDO2PINPERIPHERIALBASE, GDO2PIN);	
}

void board_gdio0_int_clear(void) {
	GPIOIntClear(GDO0PINPERIPHERIALBASE, GDO0PIN);
}

void board_gdio2_int_clear(void) {
	GPIOIntClear(GDO2PINPERIPHERIALBASE, GDO2PIN);
}

void board_delay_us (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermicrosecond * delay);
}

void board_delay_ms (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermillisecond * delay);
}

void board_init(void) {
	uint32_t clockfreq;
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
	// ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
 //                       SYSCTL_OSC_MAIN);
	clockfreq = ROM_SysCtlClockGet();
	if (clockfreq > 3e6) {
		delayloopspermicrosecond = (ROM_SysCtlClockGet() / (uint32_t)1e6) / 3;
		delayloopspermillisecond = (ROM_SysCtlClockGet() / (uint32_t)1e3) / 3;
		} else {
		//TODO: assert
	}
	GPIOIntClear(GDO0PINPERIPHERIALBASE, GDO0PIN);
	board_configure_led();
	console_init();
	board_spi_init();

	// Configure interface functions
	cc1101_interface.spi_low = board_spi_cspin_low;
	cc1101_interface.spi_write = board_spi_write;
	cc1101_interface.spi_high = board_spi_cspin_high;
	cc1101_interface.board_delay_us = board_delay_us;
	cc1101_interface.board_delay_ms = board_delay_ms;
	cc1101_interface.gdio0_edge_select = board_gdio0_edge_select;
	cc1101_interface.gdio2_edge_select = board_gdio2_edge_select;
	cc1101_interface.gdio0_int_clear = board_gdio0_int_clear;
	cc1101_interface.gdio2_int_clear = board_gdio2_int_clear;
	// cc1101_interface.gdio2_int_clear = GPIOIntClear;
	// proto: void (*gdio0_int_clear) (uint32_t port, uint32_t flags);
	// header: extern void GPIOIntClear(uint32_t ui32Port, uint32_t ui32IntFlags);
	cc1101_init(); 
	board_systick_init();

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

