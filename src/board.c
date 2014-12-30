#include <stdint.h>
#include "types.h"
#include <stdbool.h>
#include "boardconfig.h"
#include "constants.h"

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
#include "task.h"
#include "board_watchdog.h"

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
			board_delay_ms(LEDBLINKPERIODMS);
			ROM_GPIOPinWrite(LED_RED_PORTBASE, LED_RED, 0);
			break;

		case BLUE:
			ROM_GPIOPinWrite(LED_BLUE_PORTBASE, LED_BLUE, LED_BLUE);
			board_delay_ms(LEDBLINKPERIODMS);
			ROM_GPIOPinWrite(LED_BLUE_PORTBASE, LED_BLUE, 0);
			break;

		case GREEN:
			ROM_GPIOPinWrite(LED_GREEN_PORTBASE, LED_GREEN, LED_GREEN);
			board_delay_ms(LEDBLINKPERIODMS);
			ROM_GPIOPinWrite(LED_GREEN_PORTBASE, LED_GREEN, 0);
			break;
		default:
			break;
	}
	board_delay_ms(LEDBLINKPERIODMS);
}

void board_delay_us (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermicrosecond * delay);
}

void board_delay_ms (uint32_t delay) {
	ROM_SysCtlDelay(delayloopspermillisecond * delay);
}

void board_configure_gps_pins(void) {

	ROM_SysCtlPeripheralEnable(GPSPPSOUTPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTPIN);
	ROM_GPIOPadConfigSet(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTPIN, GPIO_STRENGTH_2MA, 
		GPIO_PIN_TYPE_STD_WPU);
	GPIOIntDisable(GPSPPSOUTPINPERIPHERIALBASE, GPIO0INTPIN);
    GPIOIntClear(GPSPPSOUTPINPERIPHERIALBASE, GPIO0INTPIN);
    ROM_GPIOIntTypeSet(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTPIN, GPIO_RISING_EDGE);
    GPIOIntEnable(GPSPPSOUTPINPERIPHERIALBASE, GPIO0INTPIN);
    IntEnable(GPIO0INT);

	ROM_SysCtlPeripheralEnable(GPSPUSHTOFIXPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN);
	ROM_GPIOPinWrite(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN, 0); // Switch to sleep mode

	ROM_SysCtlPeripheralEnable(GPSFIXAVAILABLEPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN);
	ROM_GPIOPadConfigSet(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN, GPIO_STRENGTH_2MA, 
		GPIO_PIN_TYPE_STD_WPU);

	ROM_SysCtlPeripheralEnable(GPSRESETPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN);
	ROM_GPIOPinWrite(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN, 0); // Stay in reset
}

void board_pushtofix_on(void) {
	ROM_GPIOPinWrite(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN, 
		GPSPUSHTOFIXPIN); // Switch to normal mode	
}

void board_pushtofix_off(void) {
	ROM_GPIOPinWrite(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN, 0); // Switch to sleep mode	
}

void board_gpsreset_assert(void) {
	ROM_GPIOPinWrite(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN, 0); // Stay in reset	
}

void board_gpsreset_deassert(void) {
	ROM_GPIOPinWrite(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN, GPSRESETPIN); // Exit reset	
}

void board_gpio0_edge_select(uint32_t edge) {
/*	uint32_t edgetype;

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

	ROM_GPIOIntTypeSet(GPIOPINPERIPHERIALBASE, GPIO0PIN, edgetype);
	*/
}

void board_gpio1_edge_select(uint32_t edge) {
/*	uint32_t edgetype;

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

	ROM_GPIOIntTypeSet(GPIOPINPERIPHERIALBASE, GPIO2PIN, edgetype);
	*/
}

static void board_configure_gpio0_pin(void){
	/*
	ROM_SysCtlPeripheralEnable(GPIOPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GPIOPINPERIPHERIALBASE, GPIO0PIN);
	ROM_GPIOPadConfigSet(GPIOPINPERIPHERIALBASE, GPIO0PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntDisable(GPIOPINPERIPHERIALBASE, GPIO0INTPIN);
    GPIOIntClear(GPIOPINPERIPHERIALBASE, GPIO0INTPIN);
    board_gdio0_edge_select(EDGE_SELECT_BOTH);
    GPIOIntEnable(GPIOPINPERIPHERIALBASE, GPIO0INTPIN);
    IntEnable(GPIO0INT);
    */ 
}

static void board_configure_gpio1_pin(void){
	/*
	ROM_SysCtlPeripheralEnable(GPIOPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GPIOPINPERIPHERIALBASE, GPIO1PIN);
	ROM_GPIOPadConfigSet(GPIOPINPERIPHERIALBASE, GPIO1PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntDisable(GPIOPINPERIPHERIALBASE, GPIO1INTPIN);
    GPIOIntClear(GPIOPINPERIPHERIALBASE, GPIO1INTPIN);
    board_gdio0_edge_select(EDGE_SELECT_BOTH);
    GPIOIntEnable(GPIOPINPERIPHERIALBASE, GPIO1INTPIN);
    IntEnable(GPIO1INT);
    */
}

void board_gpio0_int_clear(void) {
	//GPIOIntClear(GPIOPINPERIPHERIALBASE, GPIO0PIN);
}

void board_gpio1_int_clear(void) {
	//GPIOIntClear(GPIOPINPERIPHERIALBASE, GPIO1PIN);
}

void board_gpio_port_isr(void) {
	/*
	uint32_t status;
	
	status = GPIOIntStatus(GPIOPINPERIPHERIALBASE, 0);

	//check which pin has an IRQ
	if (status & GPIO0INTPIN)
		do_something();
	if (status & GPIO1INTPIN)
		do_something();
	//et cetera
	*/
}

void board_systick_isr(void) {
	task_systick();
}

void board_systick_init(void) {
	// Systick interrupt @ every 100 ms
	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / (1000/ (SYSTICKPERIODMS % 1000)));
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();
}

void board_init(void) {
	uint32_t clockfreq;
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

	board_configure_led();
	console_init();
	board_spi_init();
	board_systick_init();
	board_watchdog_init();	
	
	clockfreq = ROM_SysCtlClockGet();
	delayloopspermicrosecond = (ROM_SysCtlClockGet() / (uint32_t) 1e6) / 3;
	delayloopspermillisecond = (ROM_SysCtlClockGet() / (uint32_t) 1e3) / 3;

	console_printtext("\n\n\n***** TM4C   scheduler *****\n");
	console_printtext("**  System clock: %d MHz  **\n", ROM_SysCtlClockGet() / (uint32_t) 1e6);
	console_printtext("* Type 'help' for commands *\n");
	if (clockfreq < 3e6)
		console_printtext("WARNING: System clock is below 3 MHz, board timing might be inaccurate!\n");
	else 
		console_printtext("\n");
	
	ROM_IntMasterEnable();
}
