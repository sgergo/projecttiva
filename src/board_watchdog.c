#include <stdint.h>
#include <stdbool.h>
#include "types.h"

#include "inc/hw_memmap.h"
#include "inc/hw_watchdog.h"
#include "inc/hw_ints.h"

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/watchdog.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "board_watchdog.h"
#include "boardconfig.h"
#include "task.h"

void board_watchdog_reload(defuint_t millisec) {
	uint32_t period;

	period = ROM_SysCtlClockGet() / (1000 / (millisec % 1000));
	ROM_WatchdogReloadSet(WATCHDOGBASE, period);
}

void board_watchdog_start(void) {

	ROM_WatchdogIntClear(WATCHDOGBASE);
	ROM_IntEnable(WATCHDOGINT);
	ROM_GPIOPinWrite(WATCHDOGPINPERIPHERIALBASE, WATCHDOGPIN, WATCHDOGPIN);	
	ROM_WatchdogEnable(WATCHDOGBASE);
}

void board_watchdog_stop(void) {

	ROM_IntDisable(WATCHDOGINT);
	ROM_GPIOPinWrite(WATCHDOGPINPERIPHERIALBASE, WATCHDOGPIN, 0);	
}

void board_watchdog_isr(void) {

	ROM_WatchdogIntClear(WATCHDOGBASE);
	task_watchdog_expired ();
}
void board_watchdog_init(void) {

	ROM_SysCtlPeripheralEnable(WATCHDOGPINPERIPHERIAL);
	ROM_GPIOPinTypeGPIOOutput(WATCHDOGPINPERIPHERIALBASE, WATCHDOGPIN);
	ROM_GPIOPinWrite(WATCHDOGPINPERIPHERIALBASE, WATCHDOGPIN, 0);

	ROM_SysCtlPeripheralEnable(WATCHDOGPERIPHERIAL);

	if(ROM_WatchdogLockState(WATCHDOGBASE) == true)
		ROM_WatchdogUnlock(WATCHDOGBASE);

	//ROM_WatchdogIntRegister(WATCHDOGBASE, board_watchdog_isr);
	ROM_WatchdogIntTypeSet(WATCHDOGBASE, WATCHDOG_INT_TYPE_INT);

	//WatchdogResetEnable(WATCHDOG0_BASE);
}
