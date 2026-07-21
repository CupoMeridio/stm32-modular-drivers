#include "ds3231_oop.h"

// Static utility functions for BCD encoding/decoding
static uint8_t decode_bcd(uint8_t bin) {
	return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

static uint8_t encode_bcd(uint8_t dec) {
	return (dec % 10 + ((dec / 10) << 4));
}

int8_t ds3231_init(ds3231_t *rtc, I2C_HandleTypeDef *hi2c) {
	if (!rtc || !hi2c) {
		return DS3231_ERR;
	}

	rtc->hi2c = hi2c;
	rtc->dev_addr = (DS3231_I2C_ADDR << 1);

	// Check if the RTC device is physically present and ready on the bus
	if (HAL_I2C_IsDeviceReady(rtc->hi2c, rtc->dev_addr, 3, DS3231_TIMEOUT) != HAL_OK) {
		return DS3231_ERR;
	}

	return DS3231_OK;
}

int8_t ds3231_get_time(ds3231_t *rtc, ds3231_time_t *time) {
	if (!rtc || !rtc->hi2c || !time) {
		return DS3231_ERR;
	}

	uint8_t buffer[7];
	
	// Atomically read 7 consecutive bytes from address 0x00
	if (HAL_I2C_Mem_Read(rtc->hi2c, rtc->dev_addr, 0x00, I2C_MEMADD_SIZE_8BIT, buffer, 7, DS3231_TIMEOUT) != HAL_OK) {
		return DS3231_ERR;
	}

	// Decode time fields from BCD
	time->seconds       = decode_bcd(buffer[0] & 0x7F);
	time->minutes       = decode_bcd(buffer[1] & 0x7F);
	time->hours         = decode_bcd(buffer[2] & 0x3F); // Supports 24h mode format
	time->day_of_week   = decode_bcd(buffer[3] & 0x07);
	time->day_of_month  = decode_bcd(buffer[4] & 0x3F);
	time->month         = decode_bcd(buffer[5] & 0x1F);
	time->year          = 2000 + decode_bcd(buffer[6]); // Base century offset

	return DS3231_OK;
}

int8_t ds3231_set_time(ds3231_t *rtc, const ds3231_time_t *time) {
	if (!rtc || !rtc->hi2c || !time) {
		return DS3231_ERR;
	}

	uint8_t buffer[7];

	// Encode time fields to BCD format
	buffer[0] = encode_bcd(time->seconds) & 0x7F;
	buffer[1] = encode_bcd(time->minutes) & 0x7F;
	buffer[2] = encode_bcd(time->hours) & 0x3F; // Force 24-hour mode bit mapping
	buffer[3] = encode_bcd(time->day_of_week) & 0x07;
	buffer[4] = encode_bcd(time->day_of_month) & 0x3F;
	buffer[5] = encode_bcd(time->month) & 0x1F;
	buffer[6] = encode_bcd(time->year % 100);

	// Atomically write 7 consecutive bytes starting from address 0x00
	if (HAL_I2C_Mem_Write(rtc->hi2c, rtc->dev_addr, 0x00, I2C_MEMADD_SIZE_8BIT, buffer, 7, DS3231_TIMEOUT) != HAL_OK) {
		return DS3231_ERR;
	}

	return DS3231_OK;
}

int8_t ds3231_get_temperature(ds3231_t *rtc, float *temperature) {
	if (!rtc || !rtc->hi2c || !temperature) {
		return DS3231_ERR;
	}

	uint8_t buffer[2];

	// Read 2 bytes of temperature starting from address 0x11
	if (HAL_I2C_Mem_Read(rtc->hi2c, rtc->dev_addr, 0x11, I2C_MEMADD_SIZE_8BIT, buffer, 2, DS3231_TIMEOUT) != HAL_OK) {
		return DS3231_ERR;
	}

	int8_t temp_int = (int8_t)buffer[0];
	float temp_frac = (buffer[1] >> 6) * 0.25f;

	*temperature = (float)temp_int + temp_frac;

	return DS3231_OK;
}
