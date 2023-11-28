#include "wifi.h"
#include "mqtt.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"

#define TAG         "WIFI"
#define WIFI_SSID   "" wifi name
#define METHOD      1
#define USERNAME    "" // username
#define PASSWORD    "" // password
#define WPA_AUTH

static EventGroupHandle_t wifi_event_group;
static esp_netif_t* wifi_sta = NULL; // esp netif object representing wifi
const int CONNECTED = BIT0; 

static void event_handler(void * arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
/*************************
 description: callback function to handle any wifi event (starting, disconnecting, connecting). SHOULD NOT MANUALLY CALL UNLESS DEBUGGING. 
 arguments:
  - arg: required by any callback function
  - event_base: base of event (wifi or ip)
  - event_id: specific type of event (wifi start, wifi disconnected, wifi connected/got ip)
  - event_data: also required by any callback function, data associated with event 
 return: N/A
**************************/
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Connecting to wifi");
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected.Trying to Reconnect...");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Connected.");
        xEventGroupSetBits(wifi_event_group, CONNECTED); 
        ESP_LOGI(TAG, "Initialize MQTT Client");
        init_mqtt_client();
    }
}

void init_wifi() {
/*************************
 description: initialize the wifi and set up the netif and event groups
 arguments: N/A
 return: N/A
**************************/
    ESP_LOGI(TAG, "Initializing netif and event group.");
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "Create default wifi station with default config.");
    wifi_sta = esp_netif_create_default_wifi_sta();
    //assert(wifi_sta); // check wifi_sta

    ESP_LOGI(TAG, "Start wifi handlers.");
    wifi_init_config_t wifi_def_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_def_cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            #ifndef WPA_AUTH
            .password = PASSWORD,
            #endif
        },
    };

    ESP_LOGI(TAG, "Setting wifi mode and configuration");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    #ifdef WPA_AUTH
    ESP_LOGI(TAG, "Set EAP ID, username, and password");
    //ESP_ERROR_CHECK(esp_eap_client_set_identity((uint8_t *) ID, strlen(ID)));
    ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_username((uint8_t *) USERNAME, strlen(USERNAME)));
    ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_password((uint8_t *) PASSWORD, strlen(PASSWORD)));

    ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable()); // enable EAP authentication for WIFI
    #endif

    // Start the wifi
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Wifi started.");
}

void deinit_wifi() {
    // https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/wifi.html#wi-fi-deinit-phase
/*************************
 description: deinitialize wifi
 arguments: N/A
 return: N/A
**************************/
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    ESP_LOGI(TAG, "Deinitialized wifi.");
}



