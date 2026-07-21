#ifndef SERVO_H
#define SERVO_H

#include "main.h"

#define SERVO_OK    (0)
#define SERVO_ERR   (-1)

typedef struct servo_s {
    TIM_HandleTypeDef* htim;
    uint32_t channel;
    uint16_t current_angle;
    uint16_t target_angle;
    uint16_t min_angle;
    uint16_t max_angle;
    uint16_t step_degrees;   // Passo di rotazione in gradi (0 = istantaneo/massimo)
    uint32_t step_period_ms; // Periodo di aggiornamento del passo in ms
    uint32_t last_tick;      // Ultimo tick di movimento per l'interpolazione
} servo_t;

int8_t servo_init(servo_t* servo, TIM_HandleTypeDef* htim, uint32_t channel);
int8_t servo_start(servo_t* servo);
int8_t servo_stop(servo_t* servo);
int8_t servo_set_angle(servo_t* servo, uint16_t angle);
int8_t servo_get_angle(const servo_t* servo, uint16_t* angle);
int8_t servo_set_bounds(servo_t* servo, uint16_t min_angle, uint16_t max_angle);
void servo_update(servo_t* servo);

#endif // SERVO_H
