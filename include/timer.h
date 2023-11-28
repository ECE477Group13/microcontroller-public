#ifndef __TIMER_H__
#define __TIMER_H__

#include "esp_timer.h"
#include "esp_log.h"
#include "fsm.h"

void stop_timer();
void init_timer();
void start_timer(uint32_t num_secs);
void restart_timer(uint32_t num_secs);
void stop_ble_polling();
void start_ble_polling();
void play_audio();

#endif