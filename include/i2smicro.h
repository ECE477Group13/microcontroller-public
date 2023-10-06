//#include "driver/i2s.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"
//#include "driver/i2s_pdm.h" //Not needed?
//#include "driver/i2s_tdm.h" //Not needed?

#define I2S_DOUT GPIO_NUM_18 //Data out pin
#define I2S_BCLK 0 //Bit clock
#define I2S_LRC 0 //Left/right clock
#define I2S_PORT 0 //Port number
