#ifndef BOARD_H
#define BOARD_H

typedef enum {RED = 0, BLUE, GREEN}led_t;

void board_init(void);
void board_toggle_led(led_t led);

#endif