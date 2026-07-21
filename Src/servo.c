#include "servo.h"

int8_t servo_init(servo_t* servo, TIM_HandleTypeDef* htim, uint32_t channel)
{
    int8_t res = SERVO_ERR;
    if(servo && htim)
    {
        servo->htim = htim;
        servo->channel = channel;
        servo->current_angle = 0;
        servo->target_angle = 0;
        servo->min_angle = 0;
        servo->max_angle = 180;
        servo->step_degrees = 0;    // Default: istantaneo (velocita massima)
        servo->step_period_ms = 20; // Default: 20 ms per step
        servo->last_tick = 0;
        res = SERVO_OK;
    }
    return res;
}

int8_t servo_start(servo_t* servo)
{
    int8_t res = SERVO_ERR;
    if(servo && servo->htim)
    {
        if(HAL_TIM_PWM_Start(servo->htim, servo->channel) == HAL_OK)
        {
            res = SERVO_OK;
        }
    }
    return res;
}

int8_t servo_stop(servo_t* servo)
{
    int8_t res = SERVO_ERR;
    if(servo && servo->htim)
    {
        if(HAL_TIM_PWM_Stop(servo->htim, servo->channel) == HAL_OK)
        {
            res = SERVO_OK;
        }
    }
    return res;
}

int8_t servo_set_angle(servo_t* servo, uint16_t angle)
{
    int8_t res = SERVO_ERR;
    if(servo && servo->htim)
    {
        if(angle > servo->max_angle)
        {
            angle = servo->max_angle;
        }
        if(angle < servo->min_angle)
        {
            angle = servo->min_angle;
        }

        servo->target_angle = angle;

        // Se step_degrees e impostato a 0, il movimento e istantaneo
        if (servo->step_degrees == 0)
        {
            servo->current_angle = angle;
            uint32_t arr = __HAL_TIM_GET_AUTORELOAD(servo->htim);
            uint32_t ccr = ((45 + angle) * (arr + 1)) / 1800;
            __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
        }
        res = SERVO_OK;
    }
    return res;
}

int8_t servo_get_angle(const servo_t* servo, uint16_t* angle)
{
    int8_t res = SERVO_ERR;
    if(servo && angle)
    {
        *angle = servo->current_angle;
        res = SERVO_OK;
    }
    return res;
}

int8_t servo_set_bounds(servo_t* servo, uint16_t min_angle, uint16_t max_angle)
{
    int8_t res = SERVO_ERR;
    if(servo && min_angle <= max_angle && max_angle <= 180)
    {
        servo->min_angle = min_angle;
        servo->max_angle = max_angle;
        res = SERVO_OK;
    }
    return res;
}

void servo_update(servo_t* servo)
{
    if (servo && servo->htim && servo->step_degrees > 0)
    {
        uint32_t current_time = HAL_GetTick();
        if ((current_time - servo->last_tick) >= servo->step_period_ms)
        {
            servo->last_tick = current_time;
            if (servo->current_angle != servo->target_angle)
            {
                if (servo->current_angle < servo->target_angle)
                {
                    servo->current_angle += servo->step_degrees;
                    if (servo->current_angle > servo->target_angle)
                    {
                        servo->current_angle = servo->target_angle;
                    }
                }
                else
                {
                    servo->current_angle -= servo->step_degrees;
                    if (servo->current_angle < servo->target_angle)
                    {
                        servo->current_angle = servo->target_angle;
                    }
                }

                uint32_t arr = __HAL_TIM_GET_AUTORELOAD(servo->htim);
                uint32_t ccr = ((45 + servo->current_angle) * (arr + 1)) / 1800;
                __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);
            }
        }
    }
}
