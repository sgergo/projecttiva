#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include "boardconfig.h"
#include "console.h"
#include "console_uart.h"
#include "console_command.h"

#include "uartstdio.h"

extern uint8_t volatile console_command_verbosity_level;

void console_init(void) {
	console_uart_init(DEFAULT_BAUDRATE);
	console_command_verbosity_level = VERBOSITY_ALL; // Set default verbosity: we want to see all replies and error messages 
}

void console_printtext(const char *format , ...) {
	va_list arglist;
   	va_start( arglist, format );
   	UARTvprintf( format, arglist );
   	va_end( arglist );
}

void console_process(void) {
	static char stdin_buffer[STDINBUFFERSIZE];
	static uint32_t bytes_in;

	console_uart_process(stdin_buffer, &bytes_in);

	if (bytes_in > STDINDEFAULTCOMMANDSIZE)
		console_command_execute(stdin_buffer);
}



