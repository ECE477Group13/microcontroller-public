#ifndef _BLE_H_
#define _BLE_H_
#include <stdio.h>
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble_port.h"
#include "nimble_port_freertos.h"
#include "ble_hs.h"
#include "gap/ble_svc_gap.h"
#include "gatt/ble_svc_gatt.h"

void init_ble(void);
uint8_t ble_address_type;
void ble_app_advertise(void);

#endif