#include <stdint.h>
#include <stdbool.h>
#include "types.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "board.h"
#include "task.h"
#include "taskarg.h"

static defuint_t counter = 0;

void timer_example_isr(void) {
	static default_task_arg_t default_task_arg;

	default_task_arg.uintval = counter++;

	ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	task_addtask_by_name("ext1", 1, &default_task_arg);
	board_blink_led(BLUE);
}

void timer_example_enable (void) {

	counter = 0;
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}

void timer_example_disable (void) {
	counter = 0;
	ROM_TimerDisable(TIMER0_BASE, TIMER_A);
}

void timer_example_timer_init(void) {

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet() / 10);

	IntRegister(INT_TIMER0A, timer_example_isr);
	ROM_IntEnable(INT_TIMER0A);
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}