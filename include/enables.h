#ifndef __ENABLES_H__
#define __ENABLES_H__

#include "driver/gpio.h"

#define GPS_EN GPIO_NUM_4
#define BOOST_EN GPIO_NUM_47
#define BUZZER_EN GPIO_NUM_48

void init_enables();

#endif