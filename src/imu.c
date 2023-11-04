/*************************************************
 BoardLock - Team 13 - 2023

 File Description: IMU functions
*************************************************/
#include "imu.h"

/*************************************************
Function Description:
    Read from IMU register REG
Function Arguments:
    reg: register that is being read
    pdata: array of data to store read bytes in
    count: number of bytes
*************************************************/
esp_err_t rdLSM6DS(uint8_t reg, uint8_t *pdata, uint8_t count)
{
    return (i2c_master_rd_slave(I2C_PORT, LSM6DS_SAD0, reg, pdata, count));
}

/*************************************************
Function Description: Write to IMU register REG
Function Arguments:
    reg: register to write to
    pdata: array of data to write
    count: number of bytes
*************************************************/
esp_err_t wrLSM6DS(uint8_t reg, uint8_t *pdata, uint8_t count)
{
    return (i2c_master_wr_slave(I2C_PORT, LSM6DS_SAD0, reg, pdata, count));
}

/*************************************************
Function Description:
Function Arguments:
*************************************************/
esp_err_t init_imu()
{
    uint8_t value;
    
    // // section 4.1 of LSM6DSO32 appl note
    // value = 0x01;
    // ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_INT1_CTRL, &value, 1));

    // value = 0x60;
    // ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_CTRL1_XL, &value, 1));

    // // SECTION 5.6 OF LSM6DSO32 APPL NOTE
    value = 0x50;
    ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_CTRL1_XL, &value, 1));

    value = 0x40;
    ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_CTRL2_G, &value, 1));
    
    value = 0x00;
    ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_WAKE_UP_DUR, &value, 1));

    value = 0x01;
    ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_WAKE_UP_THS, &value, 1));

    value = 0x00;
    ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_TAP_CFG0, &value, 1));

    value = 0xE0;
    ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_TAP_CFG2, &value, 1));

    value = 0x80;
    ESP_ERROR_CHECK(wrLSM6DS(LSM6DS_MD1_CFG, &value, 1));

    return ESP_OK;
}

/*************************************************
Function Description: Retrieve acceleration from IMU
Function Arguments:
- axis: axis of desired acceleration: AXIS_X, AXIS_Y or AXIS_Z
*************************************************/
int16_t get_acc(axis_t axis) {
    uint8_t lower_reg;
    uint8_t higher_reg;
    
    uint8_t l_reg;
    uint8_t h_reg;
    
    switch(axis) {
        case AXIS_X: lower_reg = LSM6DS_OUTX_L_A; higher_reg = LSM6DS_OUTX_H_A; break;
        case AXIS_Y: lower_reg = LSM6DS_OUTY_L_A; higher_reg = LSM6DS_OUTY_H_A; break;
        case AXIS_Z: lower_reg = LSM6DS_OUTZ_L_A; higher_reg = LSM6DS_OUTZ_H_A; break;
        default: return 0;
    }
    
    rdLSM6DS(lower_reg, &(l_reg), 1);
    rdLSM6DS(higher_reg, &(h_reg), 1);
    return (h_reg << 8) | l_reg;
}

// float read_acc_float(uint8_t lower_reg, uint8_t higher_reg) {
//     uint8_t l_reg;
//     uint8_t h_reg;
//     rdLSM6DS(lower_reg, &(l_reg), 1);
//     rdLSM6DS(higher_reg, &(h_reg), 1);
//     short reg = (h_reg << 8) | l_reg;
//     // if (reg & (1<<15)) reg = reg | ((256*256-1) << 16);

//     // +- 4g, so do ratio of reg to 2^15 and then multiply by 9.8 m/s^s and 4
//     float output = 9.8 * 4 * reg / (256*128) ;
//     return output;
// }