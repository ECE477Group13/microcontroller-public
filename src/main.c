#include "main.h"
#include "i2cmicro.h"

void app_main() {    
    // Init flash memory (used for Bluetooth)
    nvs_flash_init();

    printf("waiting");
    for (int i = 0; i < 50000; i++) {
        printf(".");
    }
    printf("done waiting");
    printf("\n");

    // printf("Init_imu start\n");

    // Init I2C on Micro... do not need to touch
    uint32_t ret = init_i2c_master();

    if (ret) {
        printf("init_i2c_master: %lx\n", ret);
    }

    printf("Init_imu start\n");

    // Init imu
    init_imu();

    printf("Init_imu end\n");

    
    gpio_set_direction(GPIO_NUM_48, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_38, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_38, GPIO_PULLDOWN_ONLY);
    

    while(true) {
        
        uint8_t val; // placeholder
        uint8_t out = 0;

        //read status register 
        rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1);
        // printf("READ: %d\n", rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1));
        // printf("VAL: %x\n", val);
        if (val & (1<<0)) { // if XLDA is 1 
            uint8_t l_reg;
            uint8_t h_reg;
            rdLSM6DS(LSM6DS_OUTX_L_A, &(l_reg), 1);
            rdLSM6DS(LSM6DS_OUTX_H_A, &(h_reg), 1);
            short reg = (h_reg << 8) | l_reg;
            // if (reg & (1<<15)) reg = reg | ((256*256-1) << 16);

            // +- 4g, so do ratio of reg to 2^15 and then multiply by 9.8 m/s^s and 4
            float output = 9.8 * 4 * reg / (256*128) ;
            printf("%f m/s^2\n", output);
            if (reg > 0) {
                out = 1;
            }

        }
        
        gpio_set_level(GPIO_NUM_48, out);
        
        // gpio_set_level(GPIO_NUM_48, gpio_get_level(GPIO_NUM_38));
        // printf("Hello world!\n");
    }

    // Init ble
    //init_ble();
    
}