#include "rgb.h"

typedef struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

static color_t color = {255, 0, 255};
static bool flashing = false;

/*************************************************
Function Description: Initialize the RGB LED
Function Arguments: None
*************************************************/
void init_rgb_led(){
    // used ChatGPT

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
        .gpio_num = GPIO_NUM_40,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 3,
        .hpoint = 27,
        .flags.output_invert = 0
    };
    ledc_channel_config(&chn_config);

    /* CHANNEL 1 */
    chn_config.gpio_num = GPIO_NUM_41;
    chn_config.channel = LEDC_CHANNEL_1;
    ledc_channel_config(&chn_config);

    /* CHANNEL 2 */
    chn_config.gpio_num = GPIO_NUM_42;
    chn_config.channel = LEDC_CHANNEL_2;
    ledc_channel_config(&chn_config);

}

/*************************************************
Function Description: Set color and whether the RGB should flash 
Function Arguments:
- red: red brightness, 0x00-0xFF
- green: green brightness, 0x00-0xFF
- blue: blue brightness, 0x00-0xFF
- flash: boolean on whether to flash on and off
*************************************************/
void rgb_set_color(uint8_t red, uint8_t green, uint8_t blue, bool flash) {
    color.r = red;
    color.g = green;
    color.b = blue;
    flashing = flash;

    rgb_update_color();
}

/*************************************************
Function Description: Turns off the RGB LED
Function Arguments: None
*************************************************/
void rgb_off() {
    flashing = false;
    rgb_stop();
}

/*************************************************
Function Description: Updates the color of the RGB LED
Function Arguments: None
*************************************************/
void rgb_update_color() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, color.r);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, color.g);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, color.b);

    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
}

/*************************************************
Function Description: Turns off RGB LED without disabling flashing
Function Arguments: None
*************************************************/
void rgb_stop() {
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
}

/*************************************************
Function Description: Turns RGB LED on and off when flashing is on,
called from hled.c
Function Arguments: 
- on: bool that dictates whether to turn it on or off
*************************************************/
void rgb_toggle(bool on) {
    if (flashing) {
        if (on) {
            rgb_update_color();
        } else {
            rgb_stop();
        }
    }
}