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

    #define MAX_DUTY 255

    int red = MAX_DUTY;
    int green = 0;
    int blue = 0;

    rgb_set_color(red, green, blue);

    int stage = 0;

    while (true) {
    while (true) {

        for (int i = 0; i < MAX_DUTY; i += 10) {
            delay();

            rgb_set_color(red, green, blue);

            if (stage == 0) {
                red --;
                green ++;
                if (red == 0) stage = 1;
            } else if (stage == 1) {
                green --;
                blue ++;
                if (green == 0) stage = 2;
            } else if (stage == 2) {
                blue --;
                red ++;
                if (blue == 0) stage = 0;
            }
        }
        if (stage == 2) {
            red = MAX_DUTY;
            green = 0;
            blue = 0;
            stage = 0;
            break;
        }
    }

    rgb_off();

    for (int z = 0; z < 500; z++) {
        ESP_LOGI(TAG, ".");
    }
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