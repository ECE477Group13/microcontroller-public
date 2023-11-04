/*************************************************
 BoardLock - Team 13 - 2023

 File Description: i2s protocol functions...
*************************************************/
#include "i2cmicro.h"

/*************************************************
Function Description: I2C master read
Function Arguments:
- i2c_port: the i2c port
- i2c_addr: the address of the slave (address in i2cmicro.h)
- i2c_reg: which register in slave to read 
- data_rd: data read from the slave 
- size: number of bytes
*************************************************/
esp_err_t i2c_master_rd_slave(i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t *data_rd, size_t size)
{
    if (size == 0)
    {
        return ESP_OK;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);

    // send device address
    i2c_master_write_byte(cmd, (i2c_addr << 1) | WR_BIT, ACK_CHECK_EN);

    // send register
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);

    // send another start
    i2c_master_start(cmd);

    // send device address for read
    i2c_master_write_byte(cmd, (i2c_addr << 1) | RD_BIT, ACK_CHECK_EN);

    if (size > 1)
    {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }

    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);

    // queue stop signal
    i2c_master_stop(cmd);

    // 1, 13);

    // send queued commands
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, TICK_RATE); // last number is # of ticks to wait before timeout

    // free i2c commands
    i2c_cmd_link_delete(cmd);

    // 1, 777);

    return ret;
}

/*************************************************
Function Description: I2C master write
Function Arguments:
- i2c_port: the i2c port
- i2c_addr: the address of the slave (address in i2cmicro.h)
- i2c_reg: which register in slave to write to 
- data_rd: data to write to slave 
- size: number of bytes
*************************************************/
esp_err_t i2c_master_wr_slave(i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t *data_wr, size_t size)
{
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
Function Description: Initialize I2C on MCU
Function Arguments: N/A
*************************************************/
uint32_t init_i2c_master()
{
    /* overarching function to initialize the i2c*/

    int i2c_master_port = I2C_PORT;
    i2c_config_t config;

    // set master mode
    config.mode = I2C_MODE_MASTER;

    // set pins for SDA & SCL
    config.sda_io_num = I2C_SDA_GPIO;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE; // TODO: double check pullup or pulldown needed on IMU?
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

