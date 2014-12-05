#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "board_systick.h"
#include "console.h"

uint32_t systick_counter;

void board_systick_systickISR(void) {
    // Update the systick interrupt counter.
    systick_counter++;
    console_printtext("systick: %d\n", systick_counter);
}

void board_systick_init(void) {
	// Systick interrupt @ every second
	ROM_SysTickPeriodSet(ROM_SysCtlClockGet());
	ROM_IntMasterEnable();
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();

	systick_counter = 0;
}