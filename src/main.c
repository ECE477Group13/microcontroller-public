#include "main.h"
#include "i2smicro.h"
#include "i2cmicro.h"
#include "sd.h"

#define TAG "MAIN"



int long_loop() {
    volatile int j = 0;
    ESP_LOGI(TAG, "Start wait");
    for (int i = 0; i < 1; i++) {
        for(int k = 0; k < 10000000; k++) {
            j = (i | j) - 1 + k;
        }
    }
    ESP_LOGI(TAG, "End wait");

    // for (int i = 0; i < 5000; i++) {
    //     ESP_LOGI(TAG, ".");
    // }

    return j;
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

    gpio_num_t led1 = GPIO_NUM_47;
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Waiting");
    long_loop();
    ESP_LOGI(TAG, "Done waiting.");

    // Code goes here
    init_i2c_master();

    long_loop();
    long_loop();

    read_gps_port_config();

    while(true) {
        long_loop();

        gpio_set_level(led1, print_gps_data_stream());
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

