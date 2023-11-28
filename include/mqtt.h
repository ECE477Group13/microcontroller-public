#ifndef __MQTT_H__
#define __MQTT_H__

#define MQTT_PATH_LOCK "/BoardLock/lock"
#define MQTT_PATH_LAT "/BoardLock/lat"
#define MQTT_PATH_LON "/BoardLock/lon"
#define MQTT_PATH_BAT "/BoardLock/battery"
#define MQTT_PATH_ERROR "/BoardLock/error"

void init_mqtt_client();
int send_mqtt_data(char * path, char * data);

#endif