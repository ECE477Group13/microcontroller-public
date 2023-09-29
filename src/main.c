#include "main.h"
#include "i2cmicro.h"

void app_main() {    
    // Init flash memory (used for Bluetooth)
    //nvs_flash_init();

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

    printf("---- Init_batt_baby START ----\n");

    // Init battery babysitter
    esp_err_t init_status = init_batt_baby();

    if ( init_status == false ) {
        printf(" Failured connecting to battery babysitter. ");

    }

    printf("---- init_batt_baby END ----\n");    

    while(true) {

        //read status register 
        //rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1);
        // printf("READ: %d\n", rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1));

    }

}