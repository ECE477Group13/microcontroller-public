#ifndef __FSM_H__
#define __FSM_H__

#include <stdio.h>

typedef enum {INIT, IDLE, PAIRING, PAIRED, LOCKING, LOCKED, WARNING, ALARM, DETECT} states;

void fsm ();
void init_fsm();
void fsm_set_flag(uint8_t flag, uint8_t val);
int fsm_get_flag(uint8_t flag);

// Define flag indices
enum {
    FLAG_IND_BLE_PAIRING, // 0: do not enter pairing, 1: enter pairing 
    FLAG_IND_BLE_PAIRED, // 0: not paired, 1: paired
    FLAG_IND_LOCKED, // 0: not locked, 1: locked
    FLAG_IND_MOTION, // 0: no motion, 1: motion
    FLAG_IND_RESET, // 0: no reset, 1: reset
    FLAG_IND_TIMER, // 0: no timer set off, 1: timer set off
    FLAG_IND_MQTT, // 0: mqtt client not connected, 1: mqtt client connected
    FLAG_IND_BLE_INITED,
    FLAG_IND_WIFI_INITED,
    NUM_FLAGS
};

#endif