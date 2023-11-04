#ifndef _I2C_MICRO_H_
#define _I2C_MICRO_H_

#include <string.h>

#include "driver/i2c.h"
#include "esp_log.h"


// setup based on https://gist.github.com/mws-rmain/2ba434cd2a3f32d6d343c1c60fbd65c8
#define I2C_PORT I2C_NUM_0 // I2C port # for master development
#define I2C_SCL_GPIO 9
#define I2C_SDA_GPIO 8
#define I2C_FREQ_HZ 100000 // 0 - 100 kHz for standard I2C on the IMU

// Read/Write
#define RD_BIT I2C_MASTER_READ
#define WR_BIT I2C_MASTER_WRITE
#define ACK_CHECK_EN 0x1  // master checks ack from slave
#define ACK_CHECK_DIS 0x0 // master DOES NOT check ack from slave
#define ACK_VAL 0x0
#define NACK_VAL 0x1

#define TICK_RATE 100000000 // ms before timeout

// Function declarations
esp_err_t i2c_master_rd_slave(i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t *data_rd, size_t size);
esp_err_t i2c_master_wr_slave(i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t *data_wr, size_t size);
uint32_t init_i2c_master();
esp_err_t init_batt_baby();

#endif
