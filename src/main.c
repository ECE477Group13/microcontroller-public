#include "main.h"
#include "hled.h"
#include "i2smicro.h"
#include "i2cmicro.h"
#include "sd.h"
#include "rgb.h"
#include "imu.h"
#include "gps.h"

#define TAG "MAIN"

/*************************************************
MAIN CONTROL LOOP
*************************************************/
void app_main() {    

    /*  To turn print statements on: 
        1. Go to platformIO alien icon
        2. Press 'Run Menuconfig'
        3. Scroll to 'Component config'
        4. Scroll to 'Log output'
        5. Scroll to 'Default log verbosity'
        6. Set to 'Info' or 'No output'

        Use: ESP_LOGI(TAG, "message");
    */

    init_hbeatled();

    // ESP_LOGI(TAG, "Waiting");
    // for (int i = 0; i < 1000; i++) {
    //     ESP_LOGI(TAG, ".");
    // }
    // ESP_LOGI(TAG, "Done waiting.");

    // Code goes here
    init_i2c_master();
    // init_sd();
    // init_i2s_tx();
    // play_wav_i2s("/sdcard/wav4s.wav");
    init_imu();
    init_rgb_led();

    gpio_num_t gps_en = GPIO_NUM_4;
    gpio_set_direction(gps_en, GPIO_MODE_OUTPUT);
    gpio_set_level(gps_en, 0);

    rgb_set_color(0x04, 0x00, 0x04, true);

    uint16_t count = 0;
    while(true) {
        uint8_t val;

        //read status register 
        rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1);

        if (val & (1<<0)) { // if XLDA is 1 
            
            int16_t x_acc = get_acc(AXIS_X);
            int16_t y_acc = get_acc(AXIS_Y);
            int16_t z_acc = get_acc(AXIS_Z);
            printf("X: %d    Y: %d    Z: %d\n", x_acc, y_acc, z_acc);

            if (count >= 2000) {
                int32_t latitude;
                int32_t longitude;
                
                printf("GPS start\n");
                if (get_location(&latitude, &longitude) == ESP_OK) {
                    printf("(%ld.%ld %ld.%ld)\n", latitude / 10000000, labs(latitude) % 10000000, longitude / 10000000, labs(longitude) % 10000000);
                }
                printf("GPS end\n");

                count -= 2000;
            }
        }
        count ++;
    }

    //de_init();
    
}

/*************************************************
Function Description:
    All destroy/de-init/free functions, called
    in app_main()
Function Arguments:
    N/A
*************************************************/
void de_init(){
    unmount_sd();
    destroy_i2s_tx();
}

