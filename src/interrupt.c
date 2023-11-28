// used https://github.com/espressif/esp-idf/blob/b4268c874a4cf8fcf7c0c4153cffb76ad2ddda4e/examples/peripherals/gpio/generic_gpio/main/gpio_example_main.c

#include "interrupt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "fsm.h"
#include "esp_log.h"
#include "ble.h"
#include "esp_timer.h"
#include "battbaby.h"
#include "rgb.h"

#define TAG             "INTR"

#define EXTERNAL_BUTTON_1   GPIO_NUM_1     /* Pair-Device      */
#define EXTERNAL_BUTTON_2   GPIO_NUM_2     /* Undetermined     */
#define INTERNAL_BUTTON     GPIO_NUM_38    /* User Reset       */
#define IMU_INTERRUPT       GPIO_NUM_35    /* IMU Interrupt    */
#define N_CHG               GPIO_NUM_14    /* Battery charging      */

static QueueHandle_t gpio_evt_queue = NULL;
esp_timer_handle_t int_but_timer_handler; 

void int_but_timer_cb(void *param) { 
/*************************
 description: callback function that timer interrupt calls signal warning period over
 arguments: 
 - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
 return: N/A
**************************/
    ESP_LOGI(TAG, "DIsconnect");
    disconnect_ble();
    esp_timer_stop(int_but_timer_handler); 
}

void int_but_start_timer() {
    // ESP_LOGI(TAG, "FSM timer start start");
    esp_timer_stop(int_but_timer_handler); 
    ESP_ERROR_CHECK(esp_timer_start_once(int_but_timer_handler, 0.2 * 1000000));
}

static void IRAM_ATTR gpio_intr_handler(void *args)
{
    uint32_t pinNumber = (uint32_t) args;
    xQueueSendFromISR(gpio_evt_queue, &pinNumber, NULL);
}

static void gpio_task(void *arg) {
    uint32_t io_num;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            
            // printf("\nGPIO[%ld] intr, val: %d\n", io_num, gpio_get_level(io_num));
            switch(io_num) {
                case EXTERNAL_BUTTON_1: ext_but1_intr();    break;
                case EXTERNAL_BUTTON_2: ext_but2_intr();    break;
                case INTERNAL_BUTTON:   int_but_intr();     break;
                case IMU_INTERRUPT:     imu_intr();         break;
                case N_CHG:             batt_chg_intr();        break;
                default:                                    break;
            }
        }
    }
}

/* ----- Interrupt Init Function ----- */
/*      To be called in app_main()     */
void init_interrupts(){
    
    //interrupt init stuff goes in here, not in app_main
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = ((uint64_t) 1 << EXTERNAL_BUTTON_1) | ((uint64_t) 1 << EXTERNAL_BUTTON_2) | ((uint64_t) 1 << INTERNAL_BUTTON) | ((uint64_t) 1 << IMU_INTERRUPT);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&io_conf);

    // io_conf.pin_bit_mask = ((uint64_t) 1 << N_CHG);
    // io_conf.mode = GPIO_MODE_INPUT;
    // io_conf.intr_type = GPIO_INTR_ANYEDGE;
    // gpio_config(&io_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(gpio_task, "gpio_task", 8192, NULL, 10, NULL);

    gpio_install_isr_service(0);
    intr_enable();

    // timer
    const esp_timer_create_args_t timer_cfg = {
        .callback = &int_but_timer_cb,
        .name = "int but timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &int_but_timer_handler));
}

void intr_enable() {
    gpio_isr_handler_add(EXTERNAL_BUTTON_1, gpio_intr_handler, (void*) EXTERNAL_BUTTON_1);
    gpio_isr_handler_add(EXTERNAL_BUTTON_2, gpio_intr_handler, (void*) EXTERNAL_BUTTON_2);
    gpio_isr_handler_add(INTERNAL_BUTTON, gpio_intr_handler, (void*) INTERNAL_BUTTON);
    gpio_isr_handler_add(IMU_INTERRUPT, gpio_intr_handler, (void*) IMU_INTERRUPT);
    gpio_isr_handler_add(N_CHG, gpio_intr_handler, (void*) N_CHG);
}

void intr_disable() {
    gpio_isr_handler_remove(EXTERNAL_BUTTON_1);
    gpio_isr_handler_remove(EXTERNAL_BUTTON_2);
    gpio_isr_handler_remove(INTERNAL_BUTTON);
    gpio_isr_handler_remove(IMU_INTERRUPT);
    gpio_isr_handler_remove(N_CHG);
}

// /* ----- Bluetooth Pairing Button Interrupt ----- */
void ext_but1_intr() {
    ESP_LOGI(TAG, "ext button 1 pressed");
    fsm_set_flag(FLAG_IND_BLE_PAIRING, 1);
    // fsm_set_flag(FLAG_IND_BLE_PAIRED, 1); // TEMPORARY
    // fsm_set_flag(FLAG_IND_LOCKED, 1); // TEMPORARY
    fsm();
}

// /* ----- Erase User Data Button Interrupt ----- */
void int_but_intr() {
    int_but_start_timer();
}

// /* ----- Low Battery Interrupt ----- */
void batt_chg_intr() {
    // ESP_LOGI(TAG, "batt chg triggered");
    // if (gpio_get_level(N_CHG) == 0) {
    //     if (get_batt_percent() > 95) {
    //         rgb_set_color(0x00, 0xFF, 0x00, false, false);
    //     } else {
    //         rgb_set_color(0xFF, 0xA5, 0x00, true, false);
    //     }
    // } else {
    //     rgb_set_color(0x00, 0xA5, 0xFF, true, false);
    //     rgb_off();
    // }
}

/* ----- IMU Interrupt ----- */
void imu_intr() {
    ESP_LOGI(TAG, "imu intr triggered");
    if (!fsm_get_flag(FLAG_IND_MOTION)) {
        fsm_set_flag(FLAG_IND_MOTION, 1);
        fsm();
    }
}


// /* ----- Extra External Button Interrupt ----- */
void ext_but2_intr() {

}
