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

bool out_of_tolerance(int16_t val, int16_t val2, int16_t tol) {
    return !(val-tol <= val2 && val2 <= val+tol);
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

    gpio_num_t led = GPIO_NUM_37;
    gpio_set_direction(led, GPIO_MODE_OUTPUT);
    gpio_set_level(led, 0);

    gpio_num_t button = GPIO_NUM_1;
    gpio_set_direction(button, GPIO_MODE_INPUT);

    gpio_num_t gps_en = GPIO_NUM_4;
    gpio_set_direction(gps_en, GPIO_MODE_OUTPUT);
    // gpio_set_level(gps_en, 0);
    gpio_set_level(gps_en, 1);

    ESP_LOGI(TAG, "Waiting");
    long_loop();
    ESP_LOGI(TAG, "Done waiting.");

    // Code goes here
    init_i2c_master();
    init_imu();

    int16_t x_acc_set = 0;
    int16_t y_acc_set = 0;
    int16_t z_acc_set = 0;

    int TOLERANCE = 0x200;

    uint16_t count = 0;
    uint8_t stage = 0;
    while(true) {
        uint8_t val; // placeholder

        //read status register 
        rdLSM6DS(LSM6DS_STATUS_REG, &(val), 1);

        if (val & (1<<0)) { // if XLDA is 1 
            
            printf("X: %f m/s^2    Y: %f m/s^2    Z: %f m/s^2\n", read_acc_float(LSM6DS_OUTX_L_A, LSM6DS_OUTX_H_A), read_acc_float(LSM6DS_OUTY_L_A, LSM6DS_OUTY_H_A), read_acc_float(LSM6DS_OUTZ_L_A, LSM6DS_OUTZ_H_A));
            int16_t x_acc = read_acc(LSM6DS_OUTX_L_A, LSM6DS_OUTX_H_A);
            int16_t y_acc = read_acc(LSM6DS_OUTY_L_A, LSM6DS_OUTY_H_A);
            int16_t z_acc = read_acc(LSM6DS_OUTZ_L_A, LSM6DS_OUTZ_H_A);
            printf("X: %d    Y: %d    Z: %d    X: %d    Y: %d    Z: %d\n", x_acc, y_acc, z_acc, x_acc_set, y_acc_set, z_acc_set);

            if (gpio_get_level(button)) {
                x_acc_set = x_acc;
                y_acc_set = y_acc;
                z_acc_set = z_acc;
            }

            if (out_of_tolerance(x_acc_set, x_acc, TOLERANCE) || out_of_tolerance(y_acc_set, y_acc, TOLERANCE) || out_of_tolerance(z_acc_set, z_acc, TOLERANCE)) {
                gpio_set_level(led, 1);
            } else {
                gpio_set_level(led, 0);
            }

            if (count >= 250) {
                // ubx_send_msg(0x01, 0x02, 0, NULL);
                // print_gps_data_stream();
                print_gps_coordinates();

                count -= 250;
            }

            // if (gpio_get_level(button)) {
            //     stage ++;
            //     stage %= 2;
            //     switch(stage) {
            //         case 0:  gpio_set_level(gps_en, 0); break;
            //         case 1:  gpio_set_level(gps_en, 1); break;
            //         default: gpio_set_level(gps_en, 0); break;
            //     }
            //     for (int i = 0; i < 4000; i ++) { // change to timer interrupt
            //         printf(".");
            //     }
            // }
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

