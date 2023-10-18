#ifndef _I2S_MICRO_H_
#define _I2S_MICRO_H_

#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "freertos/freeRTOS.h" // needed for some port constants
#include "esp_log.h"
#include "esp_system.h"

#define I2S_DOUT GPIO_NUM_5 //Data out pin
#define I2S_BCLK GPIO_NUM_6 //Bit clock
#define I2S_LRC GPIO_NUM_7 //Left/right clock
#define I2S_DIN I2S_GPIO_UNUSED
#define I2S_PORT 0 //Port number

#define AUDIO_BUFFER 4096 // size of audio buffer

void init_i2s_tx();
void destroy_i2s_tx();
esp_err_t play_wav_i2s(const char * fp);
void write_i2s_tx(i2s_chan_handle_t* tx_handle, const void* src_buf, size_t bytes_to_write, size_t bytes_written, uint32_t ticks_to_wait);

#endif