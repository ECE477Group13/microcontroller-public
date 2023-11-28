#include "fsm.h"

#include "esp_log.h"

#include "nvs_flash.h"
#include "hled.h"
#include "wifi.h"
#include "ble.h"
#include "enables.h"
#include "i2smicro.h"
#include "i2cmicro.h"
#include "sd.h"
#include "timer.h"
#include "rgb.h"
#include "imu.h"
#include "interrupt.h"
#include "gps.h"
#include "battbaby.h"


#define TAG "FSM"

#define BRIGHTNESS 0xFF

static states curr_state = INIT; 
static states next_state = INIT;
static int flags[NUM_FLAGS];

void fsm () {
    
    bool entering_state = curr_state != next_state;
    curr_state = next_state;

    do {

        switch(curr_state) {
            case INIT: {
                ESP_LOGI(TAG, "INIT STATE");
                
                ble_set_err_mess("INIT STATE");

                nvs_flash_init();
                init_hbeatled();
                init_i2c_master();
                init_rgb_led();
                rgb_off();
                init_enables();
                init_imu();
                init_interrupts();
                init_timer();
                init_gps();
                init_batt_baby();

                init_sd();
                init_i2s_tx();

                next_state = IDLE;
            } break;
            case IDLE: {
                ESP_LOGI(TAG, "IDLE STATE");
                
                if (entering_state) {
                    ble_set_err_mess("IDLE STATE");
                    rgb_off();
                    ble_get_threat()[0] = 'S';
                }
                
                if (flags[FLAG_IND_BLE_PAIRING]) {
                    next_state = PAIRING; 
                }
            } break;
            case PAIRING: {
                ESP_LOGI(TAG, "PAIRING STATE");
                
                if (entering_state) {
                    ble_set_err_mess("PAIRING STATE");
    
                    ble_get_threat()[0] = 'S';
                    
                    if (!flags[FLAG_IND_BLE_INITED]) {
                        init_ble();
                        flags[FLAG_IND_BLE_INITED] = 1;
                    }

                    rgb_set_color(0x00, 0x00, BRIGHTNESS, true, false);
                }
                
                if (flags[FLAG_IND_BLE_PAIRED]) {
                    next_state = PAIRED; 
                }
            } break;
            case PAIRED: {
                ESP_LOGI(TAG, "PAIRED STATE");
                
                if (entering_state) {
                    ble_set_err_mess("PAIRED STATE");

                    ble_get_threat()[0] = 'S';
                    
                    rgb_set_color(0x00, 0x00, BRIGHTNESS, false, true);
                }
                
                if (flags[FLAG_IND_LOCKED]) {
                    next_state = LOCKING;
                }
                if (!flags[FLAG_IND_BLE_PAIRED]) {
                    // deinit_ble(); TEMPORARY: I think deinit_ble might be broke
                    flags[FLAG_IND_BLE_PAIRING] = 0;
                    next_state = IDLE;
                }
            }break;
            case LOCKING: {
                ESP_LOGI(TAG, "LOCKING STATE");

                if (entering_state) {
                    ble_set_err_mess("LOCKING STATE");

                    ble_get_threat()[0] = 'S';

                    rgb_set_color(BRIGHTNESS, 0x00, 0x00, false, true);
                    
                    start_timer(5);
                }

                if (flags[FLAG_IND_TIMER]) {
                    fsm_set_flag(FLAG_IND_TIMER, 0);
                    fsm_set_flag(FLAG_IND_MOTION, 0); // clear so we can detect motion
                    next_state = LOCKED;
                }
                if (next_state != LOCKING) {
                    rgb_off();
                }
            } break;
            case LOCKED: {
                ESP_LOGI(TAG, "LOCKED STATE");
                
                if (entering_state) {
                    ble_set_err_mess("LOCKED STATE");

                    ble_get_threat()[0] = 'S';
                }
                
                if (!flags[FLAG_IND_BLE_PAIRED]) {
                    if (!flags[FLAG_IND_WIFI_INITED]) {
                        init_wifi();
                        flags[FLAG_IND_WIFI_INITED] = 1;
                    }
                }
                if (flags[FLAG_IND_MOTION]) {
                    next_state = WARNING;
                }
                if (!flags[FLAG_IND_LOCKED]) {
                    next_state = PAIRED;
                }
            } break;
            case WARNING: {
                ESP_LOGI(TAG, "WARNING STATE");
                
                if (entering_state) {
                    ble_set_err_mess("WARNING STATE");

                    rgb_off();
                    
                    gpio_set_level(BOOST_EN, 1);

                    ble_get_threat()[0] = 'T';
                    stop_ble_polling();
                    intr_disable();
                    stop_timer();
                    start_timer(5);
                    play_wav_i2s("/sdcard/warm5.wav");
                }

                if (flags[FLAG_IND_TIMER]) {
                    next_state = DETECT;
                }
                if (!flags[FLAG_IND_LOCKED]) {
                    next_state = PAIRED;
                }

                if (next_state != WARNING) {
                    start_ble_polling();
                    intr_enable();
                    // unmount_sd();
                    // destroy_i2s_tx();
                }
            } break;
            case DETECT: {
                ESP_LOGI(TAG, "DETECT STATE");
                
                if (entering_state) {
                    ble_set_err_mess("DETECT STATE");
                    
                    ble_get_threat()[0] = 'T';
                    fsm_set_flag(FLAG_IND_MOTION, 0); // clear so we can detect motion
                    fsm_set_flag(FLAG_IND_TIMER, 0);

                    rgb_off();
                    
                    start_timer(10);
                }

                gps_start_polling();

                if (!flags[FLAG_IND_BLE_PAIRED]) {
                    if (!flags[FLAG_IND_WIFI_INITED]) {
                        init_wifi();
                        flags[FLAG_IND_WIFI_INITED] = 1;
                    }
                }
                if (flags[FLAG_IND_MOTION]) {
                    stop_timer(); // stop timer since we left state prematurely
                    fsm_set_flag(FLAG_IND_TIMER, 0);
                    next_state = ALARM;
                }
                if (flags[FLAG_IND_TIMER]) {
                    fsm_set_flag(FLAG_IND_TIMER, 0);
                    gpio_set_level(BOOST_EN, 0);
                    next_state = LOCKED;
                }
                if (!flags[FLAG_IND_LOCKED]) {
                    next_state = PAIRED;
                }
            } break;
            case ALARM: { 
                ESP_LOGI(TAG, "ALARM STATE");

                if (entering_state) {
                    ble_set_err_mess("ALARM STATE");

                    ble_get_threat()[0] = 'T';
                    fsm_set_flag(FLAG_IND_MOTION, 0);
                    fsm_set_flag(FLAG_IND_TIMER, 0);
                    
                    rgb_off();
                    gpio_set_level(BUZZER_EN, 1);
                    
                    start_timer(10);
                }

                // gps_start_polling();

                if (flags[FLAG_IND_MOTION]) {
                    stop_timer();
                    start_timer(10);
                }
                if (flags[FLAG_IND_TIMER]) {
                    fsm_set_flag(FLAG_IND_TIMER, 0);
                    gpio_set_level(BUZZER_EN, 0);
                    next_state = DETECT;
                }
                if (!flags[FLAG_IND_LOCKED]) {
                    gpio_set_level(BUZZER_EN, 0);
                    next_state = PAIRED;
                }
            } break;

        }

        entering_state = curr_state != next_state;
        curr_state = next_state;

    } while (entering_state);
}

void init_fsm() {
    // Initialize all the flags to 0
    for (int i = 0; i < NUM_FLAGS; i++) {
        flags[i] = 0;
    }

    next_state = INIT;
    fsm(flags);
}

void fsm_set_flag(uint8_t flag, uint8_t val) {
    flags[flag] = val;
}

int fsm_get_flag(uint8_t flag) {
    return flags[flag];
}