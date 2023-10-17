#include "main.h"
#include "i2cmicro.h"
#include "sd.h"

void app_main() {    
    // Init flash memory (used for Bluetooth)
    //nvs_flash_init();

    printf("waiting");
    for (int i = 0; i < 50000; i++) {
        printf(".");
    }
    printf("done waiting");
    printf("\n");


    init_sd();

    printf("finished init\n");
}