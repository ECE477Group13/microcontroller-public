#include "led.h"

// used ChatGPT

/*************************************************
Function Description:
Function Arguments:
*************************************************/
void init_rgb_led(){
    ledc_timer_config_t config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 100,
        .clk_cfg = LEDC_AUTO_CLK
    };
    
    ledc_timer_config(&config);

    /* CHANNEL 0 */
    ledc_channel_config_t chn_config = {
        .gpio_num = GPIO_NUM_47,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 3,
        .hpoint = 27,
        .flags.output_invert = 1
    };
    ledc_channel_config(&chn_config);

    /* CHANNEL 1 */
    chn_config.gpio_num = GPIO_NUM_35;
    chn_config.channel = LEDC_CHANNEL_1;
    ledc_channel_config(&chn_config);

    /* CHANNEL 2 */
    chn_config.gpio_num = GPIO_NUM_45;
    chn_config.channel = LEDC_CHANNEL_2;
    ledc_channel_config(&chn_config);

}