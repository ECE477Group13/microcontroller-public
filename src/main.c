#include "main.h"
#include "i2cmicro.h"

void app_main() {
    // Init flash module
    nvs_flash_init();

    // Init imu
    // init_imu();
    
    gpio_set_direction(GPIO_NUM_48, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_38, GPIO_MODE_INPUT);
    
    while(true) {
        gpio_set_level(GPIO_NUM_48, gpio_get_level(GPIO_NUM_38));

    }

    // Init ble
    //init_ble();
    
}