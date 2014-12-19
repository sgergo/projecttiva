#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include "boardconfig.h"

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#include "board.h"
#include "board_spi.h"
#include "console.h"
#include "cc1101.h"
#include "task.h"

extern cc1101_interface_t cc1101_interface;
uint32_t delayloopspermicrosecond;
uint32_t delayloopspermillisecond;

void board_fault(void) {
	ROM_GPIOPinWrite(LED_RED_PORTBASE, LED_RED, LED_RED);
}

static void board_configure_led(void) {
	ROM_SysCtlPeripheralEnable(LED_ALL_PINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(LED_ALL_PORTBASE, LED_RED|LED_BLUE|LED_GREEN);
	ROM_GPIOPinWrite(LED_ALL_PORTBASE, LED_RED|LED_GREEN|LED_BLUE, 0); // Switch off all LEDs
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

void board_blink_led(led_t led) {
	
	switch (led) {
		case RED:
			ROM_GPIOPinWrite(LED_RED_PORTBASE, LED_RED, LED_RED);
			board_delay_ms(50);
			ROM_GPIOPinWrite(LED_RED_PORTBASE, LED_RED, 0);
			break;

		case BLUE:
			ROM_GPIOPinWrite(LED_BLUE_PORTBASE, LED_BLUE, LED_BLUE);
			board_delay_ms(50);
			ROM_GPIOPinWrite(LED_BLUE_PORTBASE, LED_BLUE, 0);
			break;

		case GREEN:
			ROM_GPIOPinWrite(LED_GREEN_PORTBASE, LED_GREEN, LED_GREEN);
			board_delay_ms(50);
			ROM_GPIOPinWrite(LED_GREEN_PORTBASE, LED_GREEN, 0);
			break;
		default:
			break;
	}
	board_delay_ms(50);
}

void board_delay_us (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermicrosecond * delay);
}

void board_delay_ms (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermillisecond * delay);
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

static void board_configure_gdio0_pin(void){
	ROM_SysCtlPeripheralEnable(GDO0PINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GDO0PINPERIPHERIALBASE, GDO0PIN);
	ROM_GPIOPadConfigSet(GDO0PINPERIPHERIALBASE, GDO0PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntDisable(GDO0PINPERIPHERIALBASE, GDO0INTPIN);
    GPIOIntClear(GDO0PINPERIPHERIALBASE, GDO0INTPIN);
    board_gdio0_edge_select(EDGE_SELECT_BOTH);
    GPIOIntEnable(GDO0PINPERIPHERIALBASE, GDO0INTPIN);
    IntEnable(GDO0INT);
    
}

static void board_configure_gdio2_pin(void){
	ROM_SysCtlPeripheralEnable(GDO2PINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GDO2PINPERIPHERIALBASE, GDO2PIN);
	ROM_GPIOPadConfigSet(GDO2PINPERIPHERIALBASE, GDO2PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntDisable(GDO2PINPERIPHERIALBASE, GDO2PIN);
    GPIOIntClear(GDO2PINPERIPHERIALBASE, GDO2PIN);
    board_gdio2_edge_select(EDGE_SELECT_BOTH);
    GPIOIntEnable(GDO2PINPERIPHERIALBASE, GDO2PIN);
    IntEnable(GDO2INT);
}

void board_gdio0_int_clear(void) {
	GPIOIntClear(GDO0PINPERIPHERIALBASE, GDO0PIN);
}

void board_gdio2_int_clear(void) {
	GPIOIntClear(GDO2PINPERIPHERIALBASE, GDO2PIN);
}

void board_gdio_port_ISR(void) {
	uint32_t status;

	status = GPIOIntStatus(GDO0PINPERIPHERIALBASE, 0);

	if (status & GDO0INTPIN)
		cc1101_GDIO0_ISR();
	if (status & GDO2INTPIN)
		cc1101_GDIO2_ISR();
}

void board_systick_ISR(void) {
	task_systick();
}

void board_systick_init(void) {
	// Systick interrupt @ every 100 ms
	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 10);
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();
}

void board_init(void) {
	uint32_t clockfreq;
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
	
	clockfreq = ROM_SysCtlClockGet();
	if (clockfreq > 3e6) {
		delayloopspermicrosecond = (ROM_SysCtlClockGet() / (uint32_t) 1e6) / 3;
		delayloopspermillisecond = (ROM_SysCtlClockGet() / (uint32_t) 1e3) / 3;
		} else {
		//TODO: assert
	}

	board_configure_led();
	console_init();
	board_spi_init();

	// Configure interface functions
	cc1101_interface.spi_low = board_spi_cspin_low;
	cc1101_interface.spi_high = board_spi_cspin_high;
	cc1101_interface.spi_write = board_spi_write;

	cc1101_interface.board_delay_us = board_delay_us;
	cc1101_interface.board_delay_ms = board_delay_ms;

	cc1101_interface.gdio0_edge_select = board_gdio0_edge_select;
	cc1101_interface.gdio2_edge_select = board_gdio2_edge_select;
	
	cc1101_interface.gdio0_int_clear = board_gdio0_int_clear;
	cc1101_interface.gdio2_int_clear = board_gdio2_int_clear;
	
	cc1101_init();
	board_configure_gdio0_pin();
	board_configure_gdio2_pin(); 
	board_systick_init();
	console_printtext("\n\n\n**** TM4C  template ****\n");
	console_printtext("* System clock: %d MHz *\n\n", ROM_SysCtlClockGet() / (uint32_t) 1e6);
	ROM_IntMasterEnable();
}
