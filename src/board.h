#ifndef BOARD_H
#define BOARD_H

#define RED 	0
#define BLUE 	1
#define GREEN 	2
typedef uint32_t led_t;

void board_delay_us (uint32_t delay);
void board_delay_ms (uint32_t delay);
void board_init(void);
void board_toggle_led(led_t led);
void board_blink_led(led_t led);

#endif
