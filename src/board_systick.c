#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "board_systick.h"
#include "console.h"
#include "task.h"

uint32_t systick_counter;
extern taskentry_t tasktable[];
extern uint32_t volatile tasklist;

void board_systick_systickISR(void) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	//console_printtext(".");
	uint32_t taskbitshift = 0x00;
	tasklist = 0;

	while(ptr_taskentry->taskinfo) {

		if (ptr_taskentry->taskperiod > 0) {
			
			if (ptr_taskentry->periodcounter < ptr_taskentry->taskperiod) {
				ptr_taskentry->periodcounter++;
			} else {
				tasklist |=(1<<taskbitshift);
				ptr_taskentry->periodcounter = 0;
				//console_printtext("task: %s - %02x", ptr_taskentry->taskinfo, tasklist);
			}
		}

		taskbitshift++;
		ptr_taskentry++; 
    }

    //console_printtext("\n"); 
    // Update the systick interrupt counter.
    //systick_counter++;
}

void board_systick_init(void) {
	// Systick interrupt @ every 200 ms
	ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 5);
	ROM_IntMasterEnable();
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();

	systick_counter = 0;
}