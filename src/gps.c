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

#include "board_gps_uart.h"
#include "board.h"
#include "gps.h"
#include "taskarg.h"
#include "console.h"

nmea_taskarg_t nmea_taskarg;

static void gps_configure_gps_pins(void) {

	ROM_SysCtlPeripheralEnable(GPSPPSOUTPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTPIN);
	ROM_GPIOPadConfigSet(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTPIN, GPIO_STRENGTH_2MA, 
		GPIO_PIN_TYPE_STD_WPU);
	// GPIOIntDisable(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTINTPIN);
 //    GPIOIntClear(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTINTPIN);
 //    ROM_GPIOIntTypeSet(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTPIN, GPIO_RISING_EDGE);
    // GPIOIntEnable(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTINTPIN);
    // IntEnable(GPSPPSOUTINT);

	ROM_SysCtlPeripheralEnable(GPSPUSHTOFIXPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN);
	ROM_GPIOPinWrite(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN, 0); // Switch to sleep mode

	ROM_SysCtlPeripheralEnable(GPSFIXAVAILABLEPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOInput(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN);
	ROM_GPIOPadConfigSet(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN, GPIO_STRENGTH_2MA, 
		GPIO_PIN_TYPE_STD_WPU);
	// GPIOIntDisable(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEINTPIN);
 //    GPIOIntClear(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEINTPIN);
	// ROM_GPIOIntTypeSet(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN, GPIO_BOTH_EDGES);
 //    GPIOIntEnable(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEINTPIN);
 //    IntEnable(GPSFIXAVAILABLEINT);

	ROM_SysCtlPeripheralEnable(GPSRESETPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN);
	ROM_GPIOPinWrite(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN, 0); // Stay in reset
}

static void gps_pushtofix_on(void) {
	ROM_GPIOPinWrite(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN, 
		GPSPUSHTOFIXPIN); // Switch to normal mode	
}

static void gps_pushtofix_off(void) {
	ROM_GPIOPinWrite(GPSPUSHTOFIXPINPERIPHERIALBASE, GPSPUSHTOFIXPIN, 0); // Switch to sleep mode	
}

static void gps_gpsreset_assert(void) {
	ROM_GPIOPinWrite(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN, 0); // Stay in reset	
}

static void gps_gpsreset_deassert(void) {
	ROM_GPIOPinWrite(GPSRESETPINPERIPHERIALBASE, GPSRESETPIN, GPSRESETPIN); // Exit reset	
}

static void gps_gpsppsout_int_clear(void) {
	GPIOIntClear(GPSPPSOUTPINPERIPHERIALBASE, GPSPPSOUTPIN);
}

// ISR currently not used!
void gps_gpsppsout_isr(void) {
	uint32_t status;
	
	status = GPIOIntStatus(GPSPPSOUTPINPERIPHERIALBASE, 0);
	if (status & GPSPPSOUTINTPIN) {
		gps_gpsppsout_int_clear();
		// do something!
	}
}

defint_t gps_check_fixstate (void) {
	
	return ROM_GPIOPinRead(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN);
}

static void gps_fixavailable_int_clear(void) {
	GPIOIntClear(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN);

}

// ISR currently not used!
void gps_fixavailable_isr(void) {
	uint32_t status;
	uint32_t inttype;
	
	status = GPIOIntStatus(GPSFIXAVAILABLEPINPERIPHERIALBASE, 0);
	if (status & GPSFIXAVAILABLEPIN) {
		inttype = GPIOIntTypeGet(GPSFIXAVAILABLEPINPERIPHERIALBASE, GPSFIXAVAILABLEPIN);

		switch (inttype) {

			case GPIO_RISING_EDGE:
				board_led_on(GREEN);
				break;
			case GPIO_FALLING_EDGE:
				board_led_off(GREEN);
				break;
		}
		gps_fixavailable_int_clear();
	}
}

defint_t gps_getnmeasentences(void) {	
	defint_t size = 0;

	while (board_gps_uart_peek('\r') > -1) {
		size += board_gps_uart_getnmea(nmea_taskarg.nmeasentencebuffer, 
			board_gps_uart_getringbufferused());
	}
	
	if (size > 0) {
		nmea_taskarg.nmeasentencereceived = true;
		nmea_taskarg.nmeasentencesize = size;
		
	} else {
		nmea_taskarg.nmeasentencereceived = false;
		nmea_taskarg.nmeasentencesize = 0;
	}

	return size;
}

void gps_init(void) {

	board_gps_uart_init();
	gps_configure_gps_pins();
	gps_pushtofix_on();
	gps_gpsreset_deassert();
	nmea_taskarg.gpsfixed = 0x01; // Switch to unfixed
}