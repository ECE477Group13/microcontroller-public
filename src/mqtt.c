#include "mqtt.h"
#include "fsm.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"

#define BROKER_URL "ws://128.46.96.232:1883"
#define TAG "MQTT"

//https://github.com/espressif/esp-idf/blob/v5.1.1/examples/protocols/mqtt/tcp/main/app_main.c
//https://test.mosquitto.org/

static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void * event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event -> client;
    int msg_id;

    switch((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected.");
            ESP_LOGI(TAG, "Sending status of lock and gps data");
            msg_id = esp_mqtt_client_publish(client, MQTT_PATH_LOCK, "lock", strlen("lock"), 2, 0); // qos2 equals high priority
            msg_id = esp_mqtt_client_publish(client, MQTT_PATH_LAT, "gps lat", strlen("gps lat"), 1, 0); 
            msg_id = esp_mqtt_client_publish(client, MQTT_PATH_LON, "gps lon", strlen("gps lon"), 1, 0); 
            msg_id = esp_mqtt_client_publish(client, MQTT_PATH_BAT, "battery", strlen("battery"), 1, 0);

            ESP_LOGI(TAG, "Subscribing to some topics.");
            msg_id = esp_mqtt_client_subscribe(client, "/BoardLock/lock", 1);
            msg_id = esp_mqtt_client_subscribe(client, "/BoardLock/error", 1);
            
            fsm_set_flag(FLAG_IND_MQTT, 1);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            fsm_set_flag(FLAG_IND_MQTT, 0);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "Event subscribed.");
            break;
        case MQTT_EVENT_UNSUBSCRIBED: 
            ESP_LOGI(TAG, "Event %d unsubscribed", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "Event %d published", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Topic=%.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "Data=%.*s", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "error");
            break;
        default:
            break;
    }
}

esp_mqtt_client_handle_t client;
void init_mqtt_client() {
    ESP_LOGI(TAG, "Configure client");
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URL,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);

    ESP_LOGI(TAG, "Register event handler to client");
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);

    ESP_LOGI(TAG, "Starting client.");
    esp_mqtt_client_start(client);
}

int send_mqtt_data(char * path, char * data) {
    return esp_mqtt_client_publish(client, path, data, strlen(data), 1, 0);
}