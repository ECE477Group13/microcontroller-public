#ifndef _BATTBABY_H_
#define _BATTBABY_H_

#include "i2cmicro.h"
#include "BQ27441.h"

esp_err_t rdBQ27441(uint8_t reg, uint8_t *pdata, uint8_t count);
esp_err_t wrBQ27441(uint8_t reg, uint8_t *pdata, uint8_t count);
esp_err_t init_batt_baby();

#endif