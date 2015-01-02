#ifndef BOARD_GPS_UART_H
#define BOARD_GPS_UART_H


#ifndef GPS_RINGBUFFER_SIZE
#define GPS_RINGBUFFER_SIZE     1024
#endif


defint_t board_gps_uart_getringbufferused(void);
defint_t board_gps_uart_peek(unsigned char character);
defint_t board_gps_uart_getnmea(unsigned char *buf, uint32_t len);
void board_gps_uart_init(void);

#endif //BOARD_GPS_UART_H