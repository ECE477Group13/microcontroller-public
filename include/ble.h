#ifndef _BLE_H_
#define _BLE_H_

void init_ble(void);
void deinit_ble();
char* ble_get_lock();
char* ble_get_lat();
char* ble_get_lon();
char* ble_get_err_mess();
void ble_set_err_mess(char str []);
char* ble_get_threat();
char* ble_get_update();
char* ble_get_battery();
void disconnect_ble();

#endif