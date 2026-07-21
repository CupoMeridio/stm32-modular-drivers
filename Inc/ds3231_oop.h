#ifndef INC_DS3231_OOP_H_
#define INC_DS3231_OOP_H_

#include "main.h"

#define DS3231_I2C_ADDR         0x68
#define DS3231_TIMEOUT          100 // Safe non-blocking timeout in ms

#define DS3231_OK               (0)
#define DS3231_ERR              (-1)

// Structured representation of date and time
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week;    // 1 = Sunday, 7 = Saturday
    uint8_t day_of_month;   // 1 to 31
    uint8_t month;          // 1 to 12
    uint16_t year;          // Full year (e.g., 2026)
} ds3231_time_t;

// The DS3231 "Class" object
typedef struct {
    I2C_HandleTypeDef *hi2c; // Pointer to configured I2C hardware handle
    uint8_t dev_addr;        // Pre-shifted I2C address for HAL
} ds3231_t;

// Object-Oriented APIs
int8_t ds3231_init(ds3231_t *rtc, I2C_HandleTypeDef *hi2c);
int8_t ds3231_get_time(ds3231_t *rtc, ds3231_time_t *time);
int8_t ds3231_set_time(ds3231_t *rtc, const ds3231_time_t *time);
int8_t ds3231_get_temperature(ds3231_t *rtc, float *temperature);

#endif /* INC_DS3231_OOP_H_ */
