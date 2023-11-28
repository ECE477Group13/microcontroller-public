#ifndef _GPS_H_
#define _GPS_H_
#include "i2cmicro.h"
#include "SAMM8Q.h"
#include "esp_timer.h"
#include <stdio.h>
#include "enables.h"

// Function declarations
int32_t get_lat();
int32_t get_long();
int64_t get_last_pos_time();
void gps_start_polling();
void init_gps();
uint8_t print_gps_data_stream();
esp_err_t update_location();
esp_err_t ubx_send_msg(uint8_t, uint8_t id, uint16_t len, uint8_t* payload);

#endif
