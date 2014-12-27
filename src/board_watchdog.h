#ifndef BOARD_WATCHDOG_H
#define BOARD_WATCHDOG_H

void board_watchdog_reload(defuint_t millisec);
void board_watchdog_start(void);
void board_watchdog_stop(void);
void board_watchdog_isr(void);
void board_watchdog_init(void);

#endif //BOARD_WATCHDOG_H