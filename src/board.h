#ifndef BOARD_H
#define BOARD_H

typedef enum {RED = 0, BLUE, GREEN}led_t;

void board_delay_us (uint32_t delay);
void board_delay_ms (uint32_t delay);
void board_init(void);
void board_toggle_led(led_t led);
void board_blink_led(led_t led);
// void board_gdio0_int_clear(void);
// void board_gdio2_int_clear(void);

#endif