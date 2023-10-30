//referencing https://github.com/espressif/esp-idf/blob/master/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_err.h"

#define HLED_GPIO       37
#define HLED_TIMER      LEDC_TIMER_0  // 0
#define HLED_MODE       LEDC_LOW_SPEED_MODE // low or high speed
#define HLED_CHANNEL    LEDC_CHANNEL_0 // Channel 0
#define HLED_DUTY_RES   LEDC_TIMER_13_BIT // duty resolutikon is 13 bits
#define HLED_DUTY       (4096) // duty cycle 50%, (2^13) * 0.50 = 4096
#define HLED_FREQ       (5000) // frequency 5 kHz

#define TAG "HEARTBEAT LED"

ledc_timer_config_t ledc_timer = {
    .speed_mode = HLED_MODE, 
    .timer_num = HLED_TIMER,
    .duty_resolution = HLED_DUTY_RES,
    .freq_hz = HLED_FREQ,
    .clk_cfg = LEDC_AUTO_CLK
};

ledc_channel_config_t ledc_channel = {
    .speed_mode = HLED_MODE,
    .channel = HLED_CHANNEL,
    .timer_sel = HLED_TIMER, 
    .intr_type = LEDC_INTR_DISABLE,
    .gpio_num = HLED_GPIO, 
    .duty = 0,
    .hpoint = 0
};

void init_hbeatled() {
/*************************
 description: initialize a pwm signal of 5 kHz, 50% duty cycle
 arguments: N/A
 return: N/A
**************************/
    ESP_LOGI(TAG, "Starting heartbeat LED...");

    ESP_LOGI(TAG, "Applyig PWM timer config.");
    
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer)); 

    ESP_LOGI(TAG, "Applying PWM channcel config.");
    
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG, "Setting duty");
    ESP_ERROR_CHECK(ledc_set_duty(HLED_MODE, HLED_CHANNEL, HLED_DUTY));

    ESP_LOGI(TAG, "Finishied Setting Up Heartbeat LED");
}

void deinit_hbeatled() {
/*************************
 description: deinitialize LED
 arguments: N/A
 return: N/A
**************************/
    ESP_ERROR_CHECK(ledc_stop(HLED_MODE, HLED_CHANNEL, 0)); // not sure about last arg, supposed to be IDLE LEVEL
