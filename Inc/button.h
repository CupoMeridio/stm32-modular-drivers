#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_
#include "gpio.h"

#define BUTTON_INIT_DELAY		(0)

#define BUTTON_ERR		(-1)
#define BUTTON_OK		(0)

typedef enum
{
	BUTTON_RELEASED = 0,
	BUTTON_PRESSED  = 1
} button_state_t;

struct button_s
{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	button_state_t state;
	button_state_t last_state;
	uint32_t delay;
	uint32_t last_tick;
};

typedef struct button_s button_t;

int8_t button_init(button_t* button, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, button_state_t init_state);
int8_t button_read(button_t* button, button_state_t* state);
int8_t button_set_delay(button_t* button, uint32_t read_delay);
int8_t button_get_delay(button_t* button, uint32_t* read_delay);
int8_t button_get_port(const button_t* button, GPIO_TypeDef** port);
int8_t button_get_pin_number(const button_t* button, uint16_t* pin);

#endif /* INC_BUTTON_H_ */
