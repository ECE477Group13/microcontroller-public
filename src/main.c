#include "main.h"
#include "i2smicro.h"
#include "i2cmicro.h"
#include "sd.h"

#define TAG "MAIN"



int long_loop() {
    volatile int j = 0;
    ESP_LOGI(TAG, "Start wait");
    for (int i = 0; i < 1; i++) {
        for(int k = 0; k < 10000000; k++) { // 10000000
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

    gpio_num_t led1 = GPIO_NUM_45;
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Waiting");
    long_loop();
    ESP_LOGI(TAG, "Done waiting.");

    // Code goes here
    init_i2c_master();
    init_imu();

    long_loop();
    long_loop();

    uint16_t count = 0;
    while(true) {
        uint8_t val; // placeholder

        //read status register 
        rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1);

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

            if (count >= 1000) {
                //print_gps_coordinates();
                read_gps_port_config();
                print_gps_data_stream();

                count -= 1000;
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

