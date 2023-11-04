#ifndef _IMU_H_
#define _IMU_H_

#include "i2cmicro.h"
#include "LSM6DS.h"

// Function declarations
esp_err_t init_imu();
esp_err_t rdLSM6DS(uint8_t reg, uint8_t *pdata, uint8_t count);
int16_t get_acc(axis_t axis);

#endif
