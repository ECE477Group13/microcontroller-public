//referencing https://github.com/espressif/esp-idf/blob/master/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c
#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "rgb.h"

#define HLED_GPIO       37
#define DELAY           1000000// interrupt triggered every ## us


#define TAG "HEARTBEAT LED"

esp_timer_handle_t timer_handler; //timer 
static bool on; // on or off

void timer_cb(void *param) {
/*************************
 description: callback function that timer interrupt calls to set gpio high/low
 arguments: 
 - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
 return: N/A
**************************/
    
    on = !on; 
    gpio_set_level(HLED_GPIO, on);
    //ESP_LOGV(TAG, "trigger"); // only for debug, you do not want a trigger every sec
    rgb_toggle(on);
}

void init_hbeatled() {
/*************************
 description: initialize a timer interrupt on the heartbeat led
 arguments: N/A
 return: N/A
**************************/
    ESP_LOGI(TAG, "Configure %i to GPIO output", HLED_GPIO);
    esp_rom_gpio_pad_select_gpio(HLED_GPIO); // configure pin to gpio
    gpio_set_direction(HLED_GPIO, GPIO_MODE_OUTPUT); // configure for output

    // create timer
    ESP_LOGI(TAG, "Create timer.");
    const esp_timer_create_args_t timer_cfg = {
        .callback = &timer_cb,
        .name = "Heartbeat LED Timer"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &timer_handler));

    ESP_LOGI(TAG, "Starting Timer...");
    // Set timer to trigger every DELAY microseconds
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, DELAY));

    ESP_LOGI(TAG, "Heartbeat LED initialized.");
}

void deinit_hbeatled() {
/*************************
 description: deinitialize LED
 arguments: N/A
 return: N/A
**************************/
    ESP_LOGI(TAG, "Stopping the timer.");
    ESP_ERROR_CHECK(esp_timer_stop(timer_handler));
}
