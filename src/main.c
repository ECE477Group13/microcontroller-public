#include "main.h"
#include "fsm.h"
#include "esp_log.h"

// #include "nvs_flash.h"
// #include "hled.h"
// #include "i2cmicro.h"
// #include "rgb.h"
// #include "enables.h"
// #include "ble.h"
// #include "imu.h"
// #include "interrupt.h"

// #include "timer.h"

#include "i2smicro.h"
#include "sd.h"
#include "gps.h"
#include "rgb.h"
#include "battbaby.h"

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

    ESP_LOGI(TAG, "Waiting");
    for (int i = 0; i < 100; i++) {
        ESP_LOGI(TAG, ".");
    }
    ESP_LOGI(TAG, "Done waiting.");

    // init_sd();
    // init_i2s_tx();
    // play_wav_i2s("/sdcard/warm.wav");

    init_fsm();

    // gps_start_polling();

    // while(true) {
    //     int i = 0;
    //     ESP_LOGI(TAG, "waiting again\n");
    //     for (int j = 0; j < 10; j++) {
    //         for (int k = 0; k < 1000000; k ++) {
    //             i = (j + k) | i;                
    //         }
    //         printf("%d", i);
    //     }   
    // }

    // init_sd(); // TEMPORARY
    // init_i2s_tx(); // TEMPORARY
    // play_wav_i2s("/sdcard/wav4s.wav"); // TEMPORARY
}

