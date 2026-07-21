#ifndef INC_LED_H_
#define INC_LED_H_
#include "gpio.h"

#define LED_TOGGLE_PERIOD	(0)

#define LED_ERR		(-1)
#define LED_OK		(0)

typedef enum
{
	LED_OFF = 0,
	LED_ON  = 1
} led_state_t;

struct led_s
{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	led_state_t state;
	uint32_t toggle_period;
	uint32_t last_tick;
};

typedef struct led_s led_t;

int8_t led_init(led_t* led, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, led_state_t init_state);
int8_t led_write(led_t* led, led_state_t status);
int8_t led_on(led_t* led);
int8_t led_off(led_t* led);
int8_t led_set_toggle_period(led_t* led, uint32_t toggle_period);
int8_t led_get_toggle_period(led_t* led, uint32_t* toggle_period);
int8_t led_toggle(led_t* led);
int8_t led_get_status(const led_t* led, led_state_t* state);
int8_t led_get_port(const led_t* led, GPIO_TypeDef** port);
int8_t led_get_pin_number(const led_t* led, uint16_t* number);

#endif
