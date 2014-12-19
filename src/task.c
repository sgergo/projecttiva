#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "task.h"
#include "taskarg.h"
#include "cc1101.h"
#include "cc1101regs.h"

#include "console.h"
#include "command.h"

uint32_t volatile tasklist;
extern cc1101_rxpacket_t cc1101_receivedpacket;
extern cc1101_txpacket_t cc1101_sentpacket;

static void task_console_process(void *ptr_task_struct) {
	static char stdin_buffer[STDINBUFFERSIZE];
	static uint32_t bytes_in;

	console_read_uart_available_bytes(stdin_buffer, &bytes_in);

	if (bytes_in > STDINDEFAULTCOMMANDSIZE)
		command_execute(stdin_buffer);
}

static void task_sendpacket(void *ptr_task_struct) {
	cc1101_txpacket_t * const ptr_txpacketstruct = (cc1101_txpacket_t *) ptr_task_struct;

	cc1101_sendpacket(ptr_txpacketstruct->txbuf, ptr_txpacketstruct->txpacketsize, 0);
	console_printtext("...ping...\n"); 
}

static void task_receivepacket(void *ptr_task_struct) {
	cc1101_rxpacket_t * const ptr_rxpacketstruct = (cc1101_rxpacket_t *) ptr_task_struct;
	// ptr_rxpacketstruct = (rxpacket_t *) ptr_task_struct;

	cc1101_receivepacket(ptr_rxpacketstruct->rcvbuf, &ptr_rxpacketstruct->rcvpacketsize, &ptr_rxpacketstruct->rcvrssidbm, &ptr_rxpacketstruct->rcvlqi);
	if (cc1101_receivedpacket.rcvpacketsize > 0) {
		console_printtext("crc: %d received: [%s] rssi: %d size: %d\n", (ptr_rxpacketstruct->rcvlqi & 0x80)>>7, ptr_rxpacketstruct->rcvbuf, ptr_rxpacketstruct->rcvrssidbm, ptr_rxpacketstruct->rcvpacketsize);
		cc1101_receivedpacket.rcvpacketsize = 0;
	}
}

// Task table entries - fill it!
taskentry_t tasktable[] = {
	/* Entry structure:
	* short description, task function ptr, task period, periodcounter, task repetition, priority, task arg struct ptr 
	*/
	{"Send packet.", task_sendpacket, 10, 0, 0, TASKPRIORITYLEVEL_LOW, (void*)&cc1101_sentpacket}, 
	{"Receive packet.", task_receivepacket, 5, 0, 0, TASKPRIORITYLEVEL_HIGH, (void*)&cc1101_receivedpacket}, 
	{"Process console.", task_console_process, 1, 0, -1, TASKPRIORITYLEVEL_LOW, NULL},
	{0, 0, 0, 0, 0, 0, 0}
};

void task_process(void) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	uint32_t tasklist_copy;
	uint32_t i = 0;

	// Make a copy of the tasklist variable to avoid race condition
	tasklist_copy = tasklist;
	if (!tasklist_copy) return;

	while(ptr_taskentry->taskinfo) {
		if (tasklist_copy & (1<<i)) {
			ptr_taskentry->taskfunction(ptr_taskentry->taskarg);
			if (ptr_taskentry->taskrepetition > 0)
				ptr_taskentry->taskrepetition--;
			 
			// Clear task in both variables
			tasklist_copy &=~(1<<i);
			tasklist &=~(1<<i);
		} 

		i++;
		ptr_taskentry++;
	}
}

void task_systick(void) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	uint32_t i = 0;
	
	while(ptr_taskentry->taskinfo) {

		if (ptr_taskentry->taskrepetition > 0 || ptr_taskentry->taskrepetition == (-1)) {
			
			if (ptr_taskentry->periodcounter < ptr_taskentry->taskperiod) {
				ptr_taskentry->periodcounter++;
			} else { 
				tasklist |=(1<<i);
				ptr_taskentry->periodcounter = 0;
				//console_printtext("task: %s - %02x", ptr_taskentry->taskinfo, tasklist);
			}
		}

		i++;
		ptr_taskentry++; 
    }
}

int32_t task_find_task_ID( char* taskstr) {
	taskentry_t *ptr_taskentry = &tasktable[0];
	int32_t i = 0;

	while(ptr_taskentry->taskinfo) {
		if (!strcmp (ptr_taskentry->taskinfo, taskstr))
			return i;
		i++;
		ptr_taskentry++; 
	}

	return (-1);
}
