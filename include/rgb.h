#ifndef _RGB_H
#define _RGB_H

#include "driver/ledc.h"

void init_rgb_led();
void rgb_set_color(uint8_t red, uint8_t green, uint8_t blue);
void rgb_off();

#endif
