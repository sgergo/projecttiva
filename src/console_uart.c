#include <stdint.h>
#include <stdbool.h>
#include "console_uart.h"
#include "boardconfig.h"
#include "uartstdio.h"
//#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"




void console_uart_process(void) {
	uint8_t stdin_in = 0;
	static char stdin_buffer[STDINBUFFERSIZE];
	static uint8_t stdin_bufferpos;

	UARTPeek('c');

	UARTStdioIntHandler();

	while (UARTRxBytesAvail()) {

		//ha a beérkezett karakter \n vagy \r ÉS a buffer pozíció még 0, azaz semmi sincs
		//beírva ezek elé a karakterek elé, akkor kiírunk egy újsort és újból while
		stdin_in = (uint8_t) UARTgetc();
		if (stdin_in == '\n' || (stdin_in == '\r' && stdin_bufferpos == 0)) {
			UARTprintf("\n");
			continue;
		}

		//ha a bufferpozíció az utolsó előtti helyre mutat ÉS a beütött karakter nem backspace
		//enter vagy 0x7f, akkor ignoráljuk és újból while
		//azaz a buffer végénél már csak visszaléptetni, entert és 0x7f-et lehet
		if (stdin_bufferpos+1 == STDINBUFFERSIZE && stdin_in != '\b' && stdin_in != 0x7f && stdin_in != '\r')
			continue;


		if (stdin_in == '\b' || stdin_in == 0x7f) { // 0x7f - backspace
			UARTprintf("%c %c", stdin_in, stdin_in);
			if (stdin_bufferpos > 0)
				stdin_bufferpos--;
			continue;
		}

		UARTprintf("%c", stdin_in);

		if (stdin_in == '\r') {
			UARTprintf("\n");
			stdin_buffer[stdin_bufferpos] = 0;
			stdin_bufferpos = 0;
			//console_processcommand(stdin_buffer);
		} else
			stdin_buffer[stdin_bufferpos++] = stdin_in;
	}
}

void console_uart_init(void) {
    ROM_SysCtlPeripheralEnable(CONSOLEUARTPINPERIPHERIAL); // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(CONSOLEUARTPERIPHERIAL); // Enable UART0

    ROM_GPIOPinConfigure(CONSOLEUARTRXPINTYPE); // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(CONSOLEUARTTXPINTYPE);
    ROM_GPIOPinTypeUART(CONSOLEUARTPINPERIPHERIALBASE, CONSOLEUARTRXPIN | CONSOLEUARTTXPIN);

    ROM_UARTConfigSetExpClk(CONSOLEUARTPINPERIPHERIALBASE, ROM_SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    UARTStdioConfig(0, 115200, 16000000); // Initialize the UART for console I/O.
}