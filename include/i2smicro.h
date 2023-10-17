#ifndef _I2S_MICRO_H_
#define _I2S_MICRO_H_


//#include "driver/i2s.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"
//#include "driver/i2s_pdm.h" //Not needed?
//#include "driver/i2s_tdm.h" //Not needed?

#define I2S_DOUT GPIO_NUM_18 //Data out pin
#define I2S_BCLK 0 //Bit clock
#define I2S_LRC 0 //Left/right clock
#define I2S_PORT 0 //Port number

void init_i2s_tx(i2s_chan_handle_t* tx_handle);
void destroy_i2s_tx(i2s_chan_handle_t* tx_handle);
void write_i2s_tx(i2s_chan_handle_t* tx_handle, const void* src_buf, size_t bytes_to_write, size_t bytes_written, uint32_t ticks_to_wait);

#endif