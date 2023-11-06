#include "rgb.h"

typedef struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

static color_t color = {255, 0, 255};
static bool flashing = false;

/*************************************************
Function Description:
Function Arguments:
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
        .flags.output_invert = 1
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

void rgb_set_color(uint8_t red, uint8_t green, uint8_t blue, bool flash) {
    color.r = red;
    color.g = green;
    color.b = blue;
    flashing = flash;

    rgb_update_color();
}

void rgb_update_color() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, color.r);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, color.g);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, color.b);

    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
}

void rgb_off() {
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0);
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
}

void rgb_toggle(bool on) {
    if (flashing) {
        if (on) {
            rgb_update_color();
        } else {
            rgb_off();
        }
    }
}