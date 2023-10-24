#ifndef _I2C_MICRO_H_
#define _I2C_MICRO_H_

#include <string.h>

#include "driver/i2c.h"
#include "esp_log.h"

#include "LSM6DS.h"
#include "BQ27441.h"
#include "SAMM8Q.h"

// setup based on https://gist.github.com/mws-rmain/2ba434cd2a3f32d6d343c1c60fbd65c8
#define I2C_PORT I2C_NUM_0 // I2C port # for master development
#define I2C_SCL_GPIO 9
#define I2C_SDA_GPIO 8
#define I2C_FREQ_HZ 50000 // 0 - 100 kHz for standard I2C on the IMU

// Read/Write
#define RD_BIT I2C_MASTER_READ
#define WR_BIT I2C_MASTER_WRITE
#define ACK_CHECK_EN 0x1  // master checks ack from slave
#define ACK_CHECK_DIS 0x0 // master DOES NOT check ack from slave
#define ACK_VAL 0x0
#define NACK_VAL 0x1

#define TICK_RATE 100000000 // ms before timeout

// Function declarations
uint32_t init_i2c_master();
esp_err_t init_imu();
esp_err_t init_batt_baby();
esp_err_t rdLSM6DS(uint8_t reg, uint8_t *pdata, uint8_t count);
uint8_t print_gps_data_stream();
void read_gps_port_config();

#endif
