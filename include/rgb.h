#ifndef _RGB_H
#define _RGB_H

#include "driver/ledc.h"

void init_rgb_led();
void rgb_set_color(uint8_t red, uint8_t green, uint8_t blue, bool flash, bool timer_off_en);
void rgb_off();
void rgb_update_color();
void rgb_stop();
void rgb_toggle(bool on);

#endif
