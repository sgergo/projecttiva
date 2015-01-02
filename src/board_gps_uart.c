#include <stdint.h>
#include "types.h"
#include "constants.h"
#include <stdbool.h>

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "boardconfig.h"
#include "board_gps_uart.h"
#include "taskarg.h"

nmea_taskarg_t nmea_taskarg;
static unsigned char gpsringbuffer[GPS_RINGBUFFER_SIZE];
static volatile defuint_t ringbufferheadindex = 0;
static volatile defuint_t ringbuffertailindex = 0;

#define RINGBUFFER_USED          (board_gps_uart_getringbuffercount(&ringbuffertailindex,  \
                                                &ringbufferheadindex, \
                                                GPS_RINGBUFFER_SIZE))
#define RINGBUFFER_FULL          (board_gps_uart_isringbufferfull(&ringbuffertailindex,  \
                                                &ringbufferheadindex, \
                                                GPS_RINGBUFFER_SIZE))
#define RINGBUFFER_EMPTY         (board_gps_uart_isbufferempty(&ringbuffertailindex,   \
                                               &ringbufferheadindex))

#define ADVANCE_RINGBUFFER_INDEX(Index) \
                                (Index) = ((Index) + 1) % GPS_RINGBUFFER_SIZE


static bool board_gps_uart_isringbufferfull(volatile defuint_t *tail_ptr, volatile defuint_t *head_ptr, defuint_t size) {
    defuint_t head;
    defuint_t tail;

    head = *head_ptr;
    tail = *tail_ptr;

    return((((head + 1) % size) == tail) ? true : false);
} 

static defuint_t board_gps_uart_getringbuffercount(volatile defuint_t *tail_ptr, volatile defuint_t *head_ptr, defuint_t size) {
    defuint_t head;
    defuint_t tail;

    head = *head_ptr;
    tail = *tail_ptr;

    return((head >= tail) ? (head - tail) : (size - (tail - head)));
}

static bool board_gps_uart_isbufferempty(volatile defuint_t *tail_ptr, volatile defuint_t *head_ptr) {
    defuint_t head;
    defuint_t tail;

    head = *head_ptr;
    tail = *tail_ptr;

    return((head == tail) ? true : false);
}

defint_t board_gps_uart_getringbufferused(void) {
    return RINGBUFFER_USED;
}

defint_t board_gps_uart_peek(unsigned char character) {
    defint_t count;
    defint_t available;
    defuint_t readindex;

    //
    // How many characters are there in the receive buffer?
    //
    available = (defint_t)RINGBUFFER_USED;
    readindex = ringbuffertailindex;

    // Check all the unread characters looking for the one passed.
    for(count = 0; count < available; count++) {

        if(gpsringbuffer[readindex] == character) {
            // We found it so return the index
            return(count);
        } else {

            // This one didn't match so move on to the next character.
            ADVANCE_RINGBUFFER_INDEX(readindex);
        }
    }
    // If we drop out of the loop, we didn't find the character in the receive
    // buffer.
    return(-1);
}

defint_t board_gps_uart_getnmea(unsigned char *buf, uint32_t len) {
    defuint_t i = 0;
    int8_t character;

    // Adjust the length back by 1 to leave space for the trailing
    // null terminator.
    len--;


    while(1) {
        // Read the next character from the receive buffer.
        if(!RINGBUFFER_EMPTY) {
            character = gpsringbuffer[ringbuffertailindex];
            ADVANCE_RINGBUFFER_INDEX(ringbuffertailindex);

            // Process the received character as long as we are not at the end
            // of the buffer.  If the end of the buffer has been reached then
            // all additional characters are ignored until a newline is
            // received.
            if(i < len) {
                // Store the character in the caller supplied buffer.
                buf[i] = character;

                // Increment the count of characters received.
                i++;
            } else 
                break;
        }
    }

    // Add a null termination to the string.
    buf[i] = 0;

    // Return the count of int8_ts in the buffer, not counting the trailing 0.
    return(i);
}

void board_gps_uart_isr(void) {
    defuint_t status;
    unsigned char bytereceived;

    status = ROM_UARTIntStatus(GPSUARTPERIPHERIALBASE, true);
    ROM_UARTIntClear(GPSUARTPERIPHERIALBASE, status);

    if(status & (UART_INT_RX | UART_INT_RT)) {
        while(ROM_UARTCharsAvail(GPSUARTPERIPHERIALBASE)) {

            // Read a character
            bytereceived = (unsigned char) (ROM_UARTCharGet(GPSUARTPERIPHERIALBASE) & 0xFF);

            if(!RINGBUFFER_FULL) {

                // Store the new character in the receive buffer
                gpsringbuffer[ringbufferheadindex] =
                    (unsigned char)(bytereceived & 0xFF);
                ADVANCE_RINGBUFFER_INDEX(ringbufferheadindex);
            }
        }
    }
}

void board_gps_uart_init(void) {

    ROM_SysCtlPeripheralEnable(GPSUARTPINPERIPHERIAL); // Enable the GPIO Peripheral used by the UART.
    ROM_GPIOPinConfigure(GPSUARTRXPINTYPE); // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPSUARTTXPINTYPE);
    ROM_GPIOPinTypeUART(GPSUARTPINPERIPHERIALBASE, GPSUARTRXPIN | GPSUARTTXPIN);

	ROM_SysCtlPeripheralEnable(GPSUARTPERIPHERIAL);	// Enable UART peripherial
    ROM_UARTConfigSetExpClk(GPSUARTPERIPHERIALBASE, ROM_SysCtlClockGet(), 9600,
                            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_WLEN_8));
    ROM_UARTIntDisable(GPSUARTPERIPHERIALBASE, 0xFFFFFFFF);
    ROM_UARTIntEnable(GPSUARTPERIPHERIALBASE, UART_INT_RX | UART_INT_RT);
    ROM_IntEnable(INT_UART1);
    ROM_UARTEnable(GPSUARTPERIPHERIALBASE);
}