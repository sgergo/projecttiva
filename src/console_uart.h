#ifndef CONSOLE_UART_H
#define CONSOLE_UART_H

#define DEFAULT_BAUDRATE	115200

void console_uart_process(char *stdin_buffer, uint32_t *bytes_in);
void console_uart_init(uint32_t);

#endif