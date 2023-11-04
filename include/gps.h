#ifndef _GPS_H_
#define _GPS_H_

#include "i2cmicro.h"
#include "SAMM8Q.h"

// Function declarations
uint8_t print_gps_data_stream();
esp_err_t get_location(int32_t* latitude, int32_t* longitude);
esp_err_t ubx_send_msg(uint8_t, uint8_t id, uint16_t len, uint8_t* payload);

#endif
