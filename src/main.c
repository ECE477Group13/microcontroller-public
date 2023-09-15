#include "main.h"
#include "i2cmicro.h"

void app_main() {
    // Init flash module
    nvs_flash_init();

    // printf("Init_imu start\n");

    // Init I2C
    uint32_t ret = init_i2c_master();

    while (ret) {
        printf("init_i2c_master: %lx\n", ret);
    }

    // Init imu
    init_imu();

    // printf("Init_imu end\n");

    
    gpio_set_direction(GPIO_NUM_48, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_38, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_38, GPIO_PULLDOWN_ONLY);
    

    while(true) {
        
        uint8_t val; // placeholder
        uint8_t out = 0;

        //read status register 
        printf("READ: %d\n", rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1));
        printf("VAL: %x\n", val);
        if (val & (1<<0)) { // if XLDA is 1 
            rdLSM6DS(LSM6DS_OUTX_L_A, &(val), 1);
            printf("%d\n", val);
            if (val > 0) {
                out = 1;
            }

        }
        
        gpio_set_level(GPIO_NUM_48, out);
        
        gpio_set_level(GPIO_NUM_48, gpio_get_level(GPIO_NUM_38));
        // printf("Hello world!\n");
    }

    // Init ble
    //init_ble();
    
}