#include "main.h"
#include "i2smicro.h"
#include "i2cmicro.h"
#include "sd.h"
#include "rgb.h"

#define TAG "MAIN"

    #define DELAY_TIME 5

void delay() {

    for (int z = 0; z < DELAY_TIME; z++) {
                    ESP_LOGI(TAG, ".");
                }
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

    // ESP_LOGI(TAG, "Waiting");
    // for (int i = 0; i < 5000; i++) {
    //     ESP_LOGI(TAG, ".");
    // }
    // ESP_LOGI(TAG, "Done waiting.");

    // Code goes here
    init_rgb_led();

    int red = 255;
    int blue = 0;
    int green = 0;

    int stage = 0;

    while (true) {

        for (int i = 0; i < 255; i += 10) {
            delay();

            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, red);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, green);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, blue);

            if (stage == 0) {
                red --;
                blue ++;
                if (red == 0) stage = 1;
            } else if (stage == 1) {
                blue --;
                green ++;
                if (blue == 0) stage = 2;
            } else if (stage == 2) {
                green --;
                red ++;
                if (green == 0) stage = 0;
            }

            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);

        }

        // for (int i = 0; i < 255; i += 10) {
        //     for (int z = 0; z < DELAY_TIME; z++) {
        //         ESP_LOGI(TAG, ".");
        //     }

        //     for (int j = 0; j < 255; j += 10) {
        //         for (int z = 0; z < DELAY_TIME; z++) {
        //             ESP_LOGI(TAG, ".");
        //         }

        //         for (int k = 0; k < 255; k += 10) {
        //             ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
        //             ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        //             ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, j);
        //             ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        //             ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, k);
        //             ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        //             for (int z = 0; z < DELAY_TIME; z++) {
        //                 ESP_LOGI(TAG, ".");
        //             }
        //         }
        //     }
        // }
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