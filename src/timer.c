#include "timer.h"
#include "ble.h"
#include "gps.h"
#include "battbaby.h"
#include "mqtt.h"
#include "rgb.h"

// based on hled.c

#define TAG             "TIMER"
#define SEC             1000000
#define MAX_CHG_PCNT      85

static bool was_charging = false; 

esp_timer_handle_t fsm_timer_handler; //timer 
esp_timer_handle_t ble_polling_timer_handler; // TEMPORARY possibly

void stop_timer() {
    esp_timer_stop(fsm_timer_handler); // will return errer if timer isn't running but this is being ignored
    // esp_timer_delete(fsm_timer_handler);
}

void fsm_timer_cb(void *param) { 
/*************************
 description: callback function that timer interrupt calls signal warning period over
 arguments: 
 - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
 return: N/A
**************************/
    // ESP_LOGI(TAG, "FSM timer set off");

    stop_timer();

    fsm_set_flag(FLAG_IND_TIMER, 1);
    fsm();

    // ESP_LOGI(TAG, "FSM timer set off end");
}

void ble_polling_timer_cb(void *param) {  // TEMPORARY possibly
/*************************
 description: callback function that timer interrupt calls signal warning period over
 arguments: 
 - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
 return: N/A
**************************/
    if (!fsm_get_flag(FLAG_IND_LOCKED) && ble_get_lock()[0] == 'L') {
        fsm_set_flag(FLAG_IND_LOCKED, 1);
        fsm();
    }

    int32_t lat = get_lat();
    int32_t lon = get_long();

    char* lat_str = ble_get_lat();
    char* lon_str = ble_get_lon();

    lat_str[12] = '\0';
    lon_str[12] = '\0';

    if (lat < 0) {
        lat_str[0] = '-'; lat *= -1;
    } else {
        lat_str[0] = ' ';
    }
    if (lon < 0) {
        lon_str[0] = '-'; lon *= -1;
    } else {
        lon_str[0] = ' ';
    }

    for (int i = 11; i >= 1; i--) {
        if (i == 4) {
            lat_str[i] = '.';
            lon_str[i] = '.';   
        } else {
            lat_str[i] = lat % 10 + '0';
            lon_str[i] = lon % 10 + '0';

            lat /= 10;
            lon /= 10;
        }
    }

    char* batt_str = ble_get_battery(); 

    uint8_t batt_percent = get_batt_percent();

    if (batt_percent >= 100) {
        batt_str[0] = (batt_percent / 100) % 10 + '0';
        batt_str[1] = (batt_percent / 10) % 10 + '0';
        batt_str[2] = (batt_percent / 1) % 10 + '0';
        batt_str[3] = '%';
        batt_str[4] = '\0';
    } else {
        batt_str[0] = (batt_percent / 10) % 10 + '0';
        batt_str[1] = (batt_percent / 1) % 10 + '0';
        batt_str[2] = '%';
        batt_str[3] = '\0';
    }

    bool charging = get_batt_current() >= 0;

    if (batt_percent > MAX_CHG_PCNT && charging) {
        rgb_set_color(0x00, 0xFF, 0x00, false, false);
    } else if (charging && !was_charging) {
        rgb_set_color(0xFF, 0xA5, 0x00, true, false);
    } else if (!charging && was_charging) {
        rgb_off();
    } 
    was_charging = charging;


    char neg_lat = lat_str[0];
    char neg_lon = lon_str[0];
    if (fsm_get_flag(FLAG_IND_MQTT)) {
        lat_str[0] = ' ';
        lon_str[0] = ' ';
        send_mqtt_data(MQTT_PATH_LAT, lat_str);
        send_mqtt_data(MQTT_PATH_LON, lon_str);
        send_mqtt_data(MQTT_PATH_BAT, batt_str);
        lat_str[0] = neg_lat;
        lon_str[0] = neg_lon;
    }

    ble_get_update()[0] = 'Y';
    ble_get_update()[1] = '\0';
    printf("%s %s %s %s\n", ble_get_lat(), ble_get_lon(), ble_get_battery(), ble_get_err_mess());
}

void init_timer() {
    const esp_timer_create_args_t timer_cfg = {
        .callback = &fsm_timer_cb,
        .name = "fsm timer"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &fsm_timer_handler));

    const esp_timer_create_args_t polling_timer_cfg = { // TEMPORARY possibly
        .callback = &ble_polling_timer_cb,
        .name = "ble polling timer"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&polling_timer_cfg, &ble_polling_timer_handler)); // TEMPORARY possibly
    start_ble_polling(); // TEMPORARY possibly 
}

void start_timer(uint32_t num_secs) {
    // ESP_LOGI(TAG, "FSM timer start start");
    stop_timer(); // just in case
    ESP_ERROR_CHECK(esp_timer_start_once(fsm_timer_handler, num_secs * SEC));
    // ESP_LOGI(TAG, "FSM timer started");
}

void restart_timer(uint32_t num_secs) {
    ESP_ERROR_CHECK(esp_timer_restart(fsm_timer_handler, num_secs * SEC));
}

void stop_ble_polling() { // TEMPORARY possibly
    esp_timer_stop(ble_polling_timer_handler);
}

void start_ble_polling() { // TEMPORARY possibly
    esp_timer_stop(ble_polling_timer_handler);
    ESP_ERROR_CHECK(esp_timer_start_periodic(ble_polling_timer_handler, 1 * SEC));
}


#include "i2smicro.h"
#include "sd.h"

esp_timer_handle_t audio_timer_handler; // TEMPORARY possibly



// void audio_timer_cb(void *param) {  // TEMPORARY possibly
// /*************************
//  description: callback function that timer interrupt calls signal warning period over
//  arguments: 
//  - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
//  return: N/A
// **************************/

// }

// void play_audio() {
//     const esp_timer_create_args_t timer_cfg = {
//         .callback = &audio_timer_cb,
//         .name = "audio timer"
//     };
    
//     ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &audio_timer_handler));
//     ESP_ERROR_CHECK(esp_timer_start_once(audio_timer_handler, 50));
// }