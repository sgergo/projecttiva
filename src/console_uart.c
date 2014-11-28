#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "console_uart.h"
#include "boardconfig.h"
#include "uartstdio.h"
//#include "board.h"

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"


void console_uart_process(char *stdin_buffer, uint32_t *bytes_in) {	
	int f = UARTPeek('\r');
    
	if (f > -1) {
			*bytes_in = UARTRxBytesAvail();
			UARTgets(stdin_buffer, *bytes_in);
			UARTFlushRx();
	} else
		*bytes_in = 0;		
}

void console_uart_init(uint32_t baudrate) {
    ROM_SysCtlPeripheralEnable(CONSOLEUARTPINPERIPHERIAL); // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(CONSOLEUARTPERIPHERIAL); // Enable UART0

    ROM_GPIOPinConfigure(CONSOLEUARTRXPINTYPE); // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(CONSOLEUARTTXPINTYPE);
    ROM_GPIOPinTypeUART(CONSOLEUARTPINPERIPHERIALBASE, CONSOLEUARTRXPIN | CONSOLEUARTTXPIN);

    ROM_UARTConfigSetExpClk(CONSOLEUARTPINPERIPHERIALBASE, ROM_SysCtlClockGet(), baudrate,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    UARTStdioConfig(0, baudrate, 16000000); // Initialize the UART for console I/O.
}