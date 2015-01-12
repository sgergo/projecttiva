#include <stdint.h>
#include "types.h"
#include <stdbool.h>
#include "constants.h"
#include <stdio.h>
#include <string.h>

#include "task.h"
#include "taskarg.h"
#include "console.h"
#include "command.h"
#include "board_watchdog.h"
#include "board.h"

static void task_setbusystate(bool state);
static bool task_checkbusystate(void);

schedulerstate_t schedulerstate;

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
		((default_task_arg_t *)ptr_task_struct)->uintval	); 
	board_delay_ms(1500);
}

static void task_ext1(void *ptr_task_struct) {

	console_printlog(LOGTYPE_MESSAGE, "external interrupt read value: %d\n", 
		((default_task_arg_t *)ptr_task_struct)->uintval	); 
	
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
	* short description, 
	*				task function ptr, 
	*							task period, 
	*									periodcounter, 
	*											task repetition, 
	*													task arg struct ptr 
	*/
	{"example1", task_example1, 5, 0, TASKREPETITION_HALT, NULL}, 
	{"example2", task_example2, 5, 0, TASKREPETITION_HALT, NULL},
	{"example3", task_example3, 5, 0, TASKREPETITION_HALT, NULL},
	{"ext1", task_ext1, 0, 0, TASKREPETITION_HALT, NULL}, 
	{"sleep", task_sleep, 5, 0, TASKREPETITION_HALT, NULL},
	{"idle", task_idle, 1, 0, TASKREPETITION_CONTINUOUS, NULL}, 
	{0, 0, 0, 0, 0, 0}
};

static void task_setbusystate(bool state) {

	schedulerstate.taskisbusy = state;
	schedulerstate.taskticked = 0;
}

static bool task_checkbusystate(void) {
	return ((schedulerstate.taskisbusy == true));
}

static void task_start_taskdog (void) {

	board_watchdog_reload(WATCHDOGPERIODMS);
	board_watchdog_start();
}

static void task_stop_taskdog (void) {

	board_watchdog_stop();
}

void task_taskdog_expired (void) {

	if (task_checkbusystate() == true) {
		console_printlog(LOGTYPE_ERROR, "error: task execution time exceeds %d ms time-out, tasklist: %04x\n", 
			WATCHDOGPERIODMS, schedulerstate.tasklist);
	}
}

static defint_t task_resolve_taskindex (char* taskstr) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	defuint_t i = 0;

	while(ptr_taskentry->taskname) {
		if (!strcmp (ptr_taskentry->taskname, taskstr))
			return i;
		i++;
		ptr_taskentry++; 
	}

	return (-1);
}

defint_t task_launchtask (char* taskname, defint_t repetition, void *arg) {
	defint_t taskindex;

	taskindex = task_resolve_taskindex(taskname);
	if (taskindex == -1) return (-1);

	if (tasktable[taskindex].taskrepetition == 0) {
		tasktable[taskindex].taskrepetition = repetition;
		tasktable[taskindex].taskarg = arg;
	} else
		return (-1);
	
	return (0);
}

defint_t task_killtask (char* taskname) {
	defint_t taskindex;

	taskindex = task_resolve_taskindex(taskname);
	if (taskindex == -1) return (-1);
	tasktable[taskindex].taskrepetition = TASKREPETITION_HALT;
	
	return (0);
}

static void task_addtask(defuint_t taskindex) {
	
	while (schedulerstate.criticalsection == ENTERCRITICAL)
		;
	schedulerstate.criticalsection = ENTERCRITICAL;
	schedulerstate.tasklist |=(1<<taskindex);
	schedulerstate.criticalsection = EXITCRITICAL;
}

void task_addtask_by_index(defuint_t taskindex, defint_t repetition, void *arg) {
	
	tasktable[taskindex].taskrepetition = repetition;
	tasktable[taskindex].taskarg = arg;
	while (schedulerstate.criticalsection == ENTERCRITICAL)
		;
	schedulerstate.criticalsection = ENTERCRITICAL;
	schedulerstate.tasklist |=(1<<taskindex);
	schedulerstate.criticalsection = EXITCRITICAL;
}

void task_cleartask_by_index(defuint_t taskindex) {
	
	while (schedulerstate.criticalsection == ENTERCRITICAL)
		;
	schedulerstate.criticalsection = ENTERCRITICAL;
	schedulerstate.tasklist &=~(1<<taskindex);
	schedulerstate.criticalsection = EXITCRITICAL;
}

defint_t task_addtask_by_name(char* taskname, defint_t repetition, void *arg) {
	defint_t taskindex;

	taskindex = task_resolve_taskindex(taskname);
	if (taskindex == -1) return (-1);

	task_addtask_by_index(taskindex, repetition, arg);
	return (0);
}

defint_t task_cleartask_by_name(char* taskname) {
	defint_t taskindex;

	taskindex = task_resolve_taskindex(taskname);
	if (taskindex == -1) return (-1);

	task_cleartask_by_index(taskindex);
	return (0);
}

static tasklist_t task_gettasklist(void) {
	tasklist_t retval;
	
	while (schedulerstate.criticalsection)
		;
	schedulerstate.criticalsection = ENTERCRITICAL;
	retval = schedulerstate.tasklist;
	schedulerstate.criticalsection = EXITCRITICAL;
	return retval;
}

void task_systick(void) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	defuint_t i = 0;

	if (schedulerstate.taskisbusy == true) {
		schedulerstate.taskticked++;
		console_printlog(LOGTYPE_WARNING, "warning: task 0x%04x execution time is over %d ms.\n", 
			schedulerstate.tasklist, (schedulerstate.taskticked * SYSTICKPERIODMS));
		return;
	}	

	while(ptr_taskentry->taskname) {

		if (ptr_taskentry->taskrepetition > 0 || ptr_taskentry->taskrepetition == (-1)) {
			
			if (ptr_taskentry->periodcounter < ptr_taskentry->taskperiod) {
				ptr_taskentry->periodcounter++;
			} else { 
				task_addtask(i);
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

	// Get a copy of the tasklist variable to avoid race condition
	tasklist_copy = task_gettasklist();
	if (!tasklist_copy) return;

	// Arm watchdog to interrupt after WATCHDOGPERIODMS
	task_start_taskdog();

	while(ptr_taskentry->taskname) {
		if (tasklist_copy & (1<<i)) {
			task_setbusystate(true);
			ptr_taskentry->taskfunction(ptr_taskentry->taskarg);
			if (ptr_taskentry->taskrepetition > 0)
				ptr_taskentry->taskrepetition--;

			// Clear task in both variables
			task_cleartask_by_index(i);
			tasklist_copy &=~(1<<i);
			task_setbusystate(false);
		} 

		i++;
		ptr_taskentry++;
	}

	task_stop_taskdog();
}
