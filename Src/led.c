#include "led.h"

int8_t led_init(led_t* led, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, led_state_t init_state)
{
	int8_t res = LED_ERR;
	if(led)
	{
		led->GPIO_Pin = GPIO_Pin;
		led->GPIOx = GPIOx;
		led->state = init_state;
		led->toggle_period = LED_TOGGLE_PERIOD;
		led->last_tick = HAL_GetTick();
		
		GPIO_PinState pin_state = (led->state == LED_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET;
		HAL_GPIO_WritePin(led->GPIOx, led->GPIO_Pin, pin_state);
		res = LED_OK;
	}
	return res;
}

int8_t led_write(led_t* led, led_state_t status)
{
	int8_t res = LED_ERR;
	if(led)
	{
		led->state = status;
		GPIO_PinState pin_state = (led->state == LED_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET;
		HAL_GPIO_WritePin(led->GPIOx, led->GPIO_Pin, pin_state);
		res = LED_OK;
	}
	return res;
}

int8_t led_on(led_t* led)
{
	return led_write(led, LED_ON);
}

int8_t led_off(led_t* led)
{
	return led_write(led, LED_OFF);
}

int8_t led_set_toggle_period(led_t* led, uint32_t toggle_period)
{
	int8_t res = LED_ERR;
	if(led)
	{
		led->toggle_period = toggle_period;
		res = LED_OK;
	}
	return res;
}

int8_t led_get_toggle_period(led_t* led, uint32_t* toggle_period)
{
	int8_t res = LED_ERR;
	if(led)
	{
		*toggle_period = led->toggle_period;
		res = LED_OK;
	}
	return res;
}

int8_t led_toggle(led_t* led)
{
	int8_t res = LED_ERR;
	if(led)
	{
		uint32_t current_tick = HAL_GetTick();
		if((led->toggle_period == 0) || ((current_tick - led->last_tick) >= led->toggle_period))
		{
			led->last_tick = current_tick;
			led->state = (led->state == LED_ON) ? LED_OFF : LED_ON;
			GPIO_PinState pin_state = (led->state == LED_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET;
			HAL_GPIO_WritePin(led->GPIOx, led->GPIO_Pin, pin_state);
		}
		res = LED_OK;
	}
	return res;
}

inline int8_t led_get_status(const led_t* led, led_state_t* state)
{
	int8_t res = LED_ERR;
	if(led)
	{
		*state = led->state;
		res = LED_OK;
	}
	return res;
}

inline int8_t led_get_port(const led_t* led, GPIO_TypeDef** port)
{
	int8_t res = LED_ERR;
	if(led)
	{
		*port = led->GPIOx;
		res = LED_OK;
	}
	return res;
}

inline int8_t led_get_pin_number(const led_t* led, uint16_t* number)
{
	int8_t res = LED_ERR;
	if(led)
	{
		*number = led->GPIO_Pin;
		res = LED_OK;
	}
	return res;
}
