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


/*************************************************
Function Description: 
    
Function Arguments:
    
    
    
*************************************************/
esp_err_t rdBQ27441(uint8_t reg, uint8_t *pdata, uint8_t count) {
    return (i2c_master_rd_slave(I2C_PORT, BQ72441_I2C_ADDRESS, reg, pdata, count));
}

// Also change SAD0 address to new address for Baby

/*************************************************
Function Description: 
Function Arguments:
*************************************************/
esp_err_t wrBQ27441(uint8_t reg, uint8_t *pdata, uint8_t count){
    return (i2c_master_wr_slave(I2C_PORT, BQ72441_I2C_ADDRESS, reg, pdata, count));
}


/*************************************************
Function Description: 
Function Arguments:
*************************************************/
esp_err_t init_imu(){
    // section 4.1 of LSM6DSO32 appl note
    esp_err_t err;
    
    uint8_t value = 01;
    err = wrLSM6DS(LSM6DS_INT1_CTRL, &value, 1);
    if(err != 0) return err;
    
    value = 0x60;
    err = wrLSM6DS(LSM6DS_CTRL1_XL, &value, 1);
    return err;
}


/*************************************************
Function Description: 
Function Arguments:
*************************************************/
esp_err_t init_batt_baby(){
    uint8_t deviceID [2];
    
    //Wire.begin(); // Initialize I2C master
    
    //deviceID = deviceType(); // Read deviceType from BQ27441

    deviceID[0] = 0x80;
    deviceID[1] = 0x00;
    

    while(true) {
        wrBQ27441(BQ27441_COMMAND_CONTROL, deviceID, 2);
        rdBQ27441(BQ27441_COMMAND_CONTROL, deviceID, 2);
        printf("%x %x", deviceID[0], deviceID[1]);
    }

    uint8_t val;
    val = 0x00;
    printf("%x\n", wrBQ27441(BQ27441_COMMAND_CONTROL, &val, 1));
    val = 0x80;
    printf("%x\n", wrBQ27441(0x01, &val, 1));
    val = 0x00;
    printf("%x\n", wrBQ27441(BQ27441_COMMAND_CONTROL, &val, 1));
    val = 0x80;
    printf("%x\n", wrBQ27441(0x01, &val, 1));



    val = 0x13;
    printf("%x\n", wrBQ27441(BQ27441_COMMAND_CONTROL, &val, 1));
    val = 0x00;
    printf("%x\n", wrBQ27441(0x01, &val, 1));

    while((val & (1<<4)) == 0) {
        val = 0x00;
        printf("%x\n", rdBQ27441(BQ27441_COMMAND_FLAGS, &val, 1));
        printf("val %x\n", val);
    }

    val = 0x00;
    printf("%x\n", wrBQ27441(BQ27441_EXTENDED_CONTROL, &val, 1));


    val = 0x52;
    printf("%x\n", wrBQ27441(0x3E, &val, 1));
    
    val = 0x00;
    printf("%x\n", wrBQ27441(0x3F, &val, 1));
    
    val = 0x00;
    printf("%x\n", rdBQ27441(0x60, &val, 1));
    printf("val2 %x\n", val);


    esp_err_t result = rdBQ27441(BQ27441_CONTROL_DEVICE_TYPE, deviceID, 2);
    printf("%x\n", result);
    printf("%x %x\n", deviceID[0], deviceID[1]);

    if( result == 0){
        return result;
    }

    return result;

}