#include "i2cmicro.h"


/*************************************************
Function Description: 
Function Arguments:
*************************************************/
void debug_print(int val, uint16_t print_num) {
    if (true) {
        if (val != 0) {
            // while(true) {
                printf("#%d printout: %d\n", print_num, val);
            // }
        }
    }
}


/*************************************************
Function Description: 
Function Arguments:
*************************************************/
static esp_err_t i2c_master_rd_slave (i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_rd, size_t size){
    if (size == 0) {
        return ESP_OK; 
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if(cmd == NULL) debug_print(1, 4);
    debug_print(i2c_master_start(cmd), 5);

    // send device address
    debug_print(i2c_master_write_byte(cmd, (i2c_addr << 1) | WR_BIT, ACK_CHECK_EN), 6);

    // send register
    debug_print(i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN), 7);

    // send another start
    debug_print(i2c_master_start(cmd), 8);

    // send device address for read
    debug_print(i2c_master_write_byte(cmd, (i2c_addr << 1) | RD_BIT, ACK_CHECK_EN), 9);

    if (size > 1) {
        debug_print(i2c_master_read(cmd, data_rd, size - 1, ACK_VAL), 10);
    }

    debug_print(i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL), 11);

    // queue stop signal
    debug_print(i2c_master_stop(cmd), 12);

    // debug_print(1, 13);

    // send queued commands
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, TICK_RATE); // last number is # of ticks to wait before timeout
    debug_print(ret, 201);

    // free i2c commands
    i2c_cmd_link_delete(cmd);

    // debug_print(1, 777);

    return ret;
}


/*************************************************
Function Description: 
Function Arguments:
*************************************************/
static esp_err_t i2c_master_wr_slave (i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_wr, size_t size){
    // create command queue
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // queue a start command
    i2c_master_start(cmd);

    // send device address
    i2c_master_write_byte(cmd, (i2c_addr << 1) | WR_BIT, ACK_CHECK_EN);

    // send register addresss
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);

    // write the data
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);

    // queue a stop command
    i2c_master_stop(cmd);

    // send commands
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, TICK_RATE);

    i2c_cmd_link_delete(cmd);

    return ret;
}


/*************************************************
Function Description: 
Function Arguments:
*************************************************/
uint32_t init_i2c_master(){
    /* overarching function to initialize the i2c*/

    int i2c_master_port = I2C_PORT;
    i2c_config_t config;

    // set master mode
    config.mode = I2C_MODE_MASTER;

    // set pins for SDA & SCL
    config.sda_io_num = I2C_SDA_GPIO;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE; //TODO: double check pullup or pulldown needed on IMU?
    config.scl_io_num = I2C_SCL_GPIO;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE; 

    // set clock with the bus frequency
    config.master.clk_speed = I2C_FREQ_HZ; 
    config.clk_flags = 0;

    uint32_t ret = 0;

    // Configure parameters
    ret |= i2c_param_config(i2c_master_port, &config) << 16;

    // begin
    ret |= i2c_driver_install(i2c_master_port, config.mode, 0, 0, 0);

   return ret; 
}


/*************************************************
Function Description: 
    Read from IMU register REG
Function Arguments:
    reg: register that is being read
    pdata: array of data to store read bytes in
    count: number of bytes
*************************************************/
esp_err_t rdLSM6DS(uint8_t reg, uint8_t *pdata, uint8_t count) {
    return (i2c_master_rd_slave(I2C_PORT, LSM6DS_SAD0, reg, pdata, count));
}


/*************************************************
Function Description: Write to IMU register REG
Function Arguments:
*************************************************/
esp_err_t wrLSM6DS(uint8_t reg, uint8_t *pdata, uint8_t count){
    return (i2c_master_wr_slave(I2C_PORT, LSM6DS_SAD0, reg, pdata, count));
}


//NICK
// Modify for the Battery Babysitter (this is copied from above)

// esp_err_t rdBaby(uint8_t reg, uint8_t *pdata, uint8_t count) {
//     return (i2c_master_rd_slave(I2C_PORT, SAD0, reg, pdata, count));
// }

// Also change SAD0 address to new address for Baby

// esp_err_t wrBaby(uint8_t reg, uint8_t *pdata, uint8_t count){
//     return (i2c_master_wr_slave(I2C_PORT, SAD0, reg, pdata, count));
// }


/*************************************************
Function Description: 
Function Arguments:
*************************************************/
void init_imu(){
    // section 4.1 of LSM6DSO32 appl note
    uint8_t value = 01;
    debug_print(wrLSM6DS(LSM6DS_INT1_CTRL, &value, 1), -999);
    value = 0x60;
    debug_print(wrLSM6DS(LSM6DS_CTRL1_XL, &value, 1), -998);

    // printf("IM here");
    // debug_print(rdLSM6DS(LSM6DS_FUNC_CFG_ACCESS, &value, 1), -997);
    // value |= 1<<7;
    // debug_print(wrLSM6DS(LSM6DS_FUNC_CFG_ACCESS, &value, 1), -996);

    // printf("now Im here");
    // debug_print(rdLSM6DS(LSM6DS_EMB_FUNC_INT2, &value, 1), -995);
    // value |= 1<<5;
    // debug_print(wrLSM6DS(LSM6DS_EMB_FUNC_INT2, &value, 1), -994);
    
    // // section 5.6 of the IMU Application Note 
    // uint8_t val; // placeholder 
    // // disable IMU, write (I2C_disable) = 1 to CTRL4_C
    // debug_print(rdLSM6DS(LSM6DS_CTRL4_C, &(val), 1), 1);
    // val |= I2C_disable;
    // debug_print(wrLSM6DS(LSM6DS_CTRL4_C, &(val), 1), 2);

    // // who am i
    // rdLSM6DS(LSM6DS_WHOAMI, &(val), 1);
    // if (val == 0x6C) {
    //     ESP_LOGI("I2C_restart", "Initializing LSM6DSO32 ID:0x%X", val);
    // }

    // // set ODR_XL[3:0] in the CTRL1_XL reg for accelerometer ODR max is 3.3 kHz, also turns on accelerometer
    // lsm6ds_data_rate_t data_rate = LSM6DS_RATE_3_33K_HZ;
    // val = ((uint8_t) data_rate) << 4;
    // wrLSM6DS(LSM6DS_CTRL1_XL, &(val), 1);

    // // set WAKE_UP_DUR to set inactivity duration
    // val = 0x02;
    // wrLSM6DS(LSM6DS_WAKEUP_DUR, &(val), 1);

    // // set inactivty and activity threshold in WAKE_UP_THS
    // val = 0x01;
    // wrLSM6DS(LSM6DS_WAKEUP_THS, &(val), 1);

    // // set INTERRUPT_ENABLE bit to 1 in TAP_CFG reg
    

    // // set pin for Interrupt in MD1_CFG
}

/*************************************************
Function Description: 
Function Arguments:
*************************************************/
void init_batt_baby(){
    // section 4.1 of LSM6DSO32 appl note
    // uint8_t value = 01;
    // debug_print(wrLSM6DS(LSM6DS_INT1_CTRL, &value, 1), -999);
    // value = 0x60;
    // debug_print(wrLSM6DS(LSM6DS_CTRL1_XL, &value, 1), -998);

}