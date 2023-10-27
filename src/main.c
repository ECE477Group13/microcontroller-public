#include "main.h"
#include "i2smicro.h"
#include "i2cmicro.h"
#include "sd.h"

#define TAG "MAIN"


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
    for (int i = 0; i < 5000; i++) {
        ESP_LOGI(TAG, ".");
    }
    ESP_LOGI(TAG, "Done waiting.");

    // Code goes here
    // init_i2c_master();
    // init_sd();
    // init_i2s_tx();
    // play_wav_i2s("/sdcard/nat1m.wav");

    de_init();
    
}

