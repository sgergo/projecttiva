#include <stdint.h>
#include "types.h"
#include "constants.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "task.h"
#include "taskarg.h"
#include "console.h"
#include "command.h"
#include "board_watchdog.h"
#include "board_gps_uart.h"
#include "board.h"

tasklist_t volatile tasklist;
flag_t criticalsection;
extern nmea_taskarg_t nmea_taskarg;

static void task_sleep(void *ptr_task_struct) {

	console_printlog(LOGTYPE_MESSAGE, "sleep.\n"); 
}

static void task_example1(void *ptr_task_struct) {

	console_printlog(LOGTYPE_MESSAGE, "example1 task.\n"); 
}

static void task_example2(void *ptr_task_struct) {

	console_printlog(LOGTYPE_MESSAGE, "example2 task.\n"); 
}

static void task_example3(void *ptr_task_struct) {

	console_printlog(LOGTYPE_MESSAGE, "example3 task with value: 0x%02x\n", 
		((default_taskarg_t *)ptr_task_struct)->uintval	); 
	board_delay_ms(100);
}

static void task_gpsfix(void *ptr_task_struct) {

	((nmea_taskarg_t *) ptr_task_struct)->gpsfixed <<= 1;
				
	if (!gps_check_fixstate ())
			((nmea_taskarg_t *) ptr_task_struct)-> gpsfixed |=0x01;
}

static void task_nmea_output(void *ptr_task_struct) {

	gps_getnmeasentences();

	if (((nmea_taskarg_t *) ptr_task_struct)->nmeasentencereceived == false)
		return;

	board_toggle_led(BLUE);

	if (!((nmea_taskarg_t *) ptr_task_struct)->gpsfixed || 
		!((nmea_taskarg_t *) ptr_task_struct)->fixfilter) {

		console_printlog(LOGTYPE_MESSAGE, "%s",	
			((nmea_taskarg_t *) ptr_task_struct)->nmeasentencebuffer);
	}
}

static void task_idle(void *ptr_task_struct) {
	static char stdin_buffer[STDINBUFFERSIZE];
	static defuint_t bytes_in;

	console_read_uart_available_bytes(stdin_buffer, &bytes_in);

	if (bytes_in > STDINDEFAULTCOMMANDSIZE)
		command_execute(stdin_buffer);
}

// Task table entries - fill it!
taskentry_t tasktable[] = {
	/* Entry structure:
	* short description, task function ptr, task period, periodcounter, task repetition, priority, task arg struct ptr 
	*/
	{"idle", task_idle, 2, 0, TASKREPETITION_CONTINUOUS, TASKPRIORITYLEVEL_HIGH, NULL}, 
	{"example1", task_example1, 5, 0, 0, TASKPRIORITYLEVEL_HIGH, NULL}, 
	{"example2", task_example2, 5, 0, 0, TASKPRIORITYLEVEL_HIGH, NULL},
	{"example3", task_example3, 5, 0, 0, TASKPRIORITYLEVEL_HIGH, NULL},
	{"gpsfix", task_gpsfix, 2, 0, 0, TASKPRIORITYLEVEL_HIGH, NULL},
	{"nmea_output", task_nmea_output, 1, 0, 0, TASKPRIORITYLEVEL_HIGH, NULL},
	{"sleep", task_sleep, 5, 0, 0, TASKPRIORITYLEVEL_LOW, NULL},
	{0, 0, 0, 0, 0, 0, 0}
};

defint_t task_find_task_ID_by_infostring( char* taskstr) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	defuint_t i = 0;

	while(ptr_taskentry->taskinfo) {
		if (!strcmp (ptr_taskentry->taskinfo, taskstr))
			return i;
		i++;
		ptr_taskentry++; 
	}

	return (-1);
}

static void task_start_watchdog (defuint_t millisec) {

	board_watchdog_reload(millisec);
	board_watchdog_start();
}

void task_watchdog_expired (void) {
	if (criticalsection != EXITCRITICAL) {
		console_printlog(LOGTYPE_ERROR, "error: systick period violation, tasklist: %04x\n", tasklist);
		tasklist = 0;	
	}
}

void task_systick(void) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	defuint_t i = 0;
	
	while(ptr_taskentry->taskinfo) {

		if (ptr_taskentry->taskrepetition > 0 || ptr_taskentry->taskrepetition == (-1)) {
			
			if (ptr_taskentry->periodcounter < ptr_taskentry->taskperiod) {
				ptr_taskentry->periodcounter++;
			} else { 
				tasklist |=(1<<i);
				ptr_taskentry->periodcounter = 0;
			}
		}

		i++;
		ptr_taskentry++; 
    }
}

void task_process(void) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	tasklist_t tasklist_copy;
	defuint_t i = 0;

	// Make a copy of the tasklist variable to avoid race condition
	tasklist_copy = tasklist;
	if (!tasklist_copy) return;

	// Arm watchdog to interrupt 10 ms before the next systick
	task_start_watchdog(SYSTICKPERIODMS - 10);
	while(ptr_taskentry->taskinfo) {
		if (tasklist_copy & (1<<i)) {
			//console_printlog(LOGTYPE_MESSAGE, "tlb: %04x  ", tasklist);
			criticalsection = ENTERCRITICAL;
			ptr_taskentry->taskfunction(ptr_taskentry->taskarg);
			if (ptr_taskentry->taskrepetition > 0)
				ptr_taskentry->taskrepetition--;
			 
			// Clear task in both variables
			tasklist_copy &=~(1<<i);
			tasklist &=~(1<<i);
			criticalsection = EXITCRITICAL;
			//console_printlog(LOGTYPE_MESSAGE, "tla: %04x\n", tasklist);
		} 

		i++;
		ptr_taskentry++;
	}
	board_watchdog_stop();
}
