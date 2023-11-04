/*************************************************
 BoardLock - Team 13 - 2023

 File Description: Battery baby functions
*************************************************/
#include "battbaby.h"

/*************************************************
Function Description: Read to Battery babysitter register, reg
Function Arguments:
    reg: register that is being read
    pdata: array of data to store read bytes in
    count: number of bytes
*************************************************/
esp_err_t rdBQ27441(uint8_t reg, uint8_t *pdata, uint8_t count)
{
    return (i2c_master_rd_slave(I2C_PORT, BQ72441_I2C_ADDRESS, reg, pdata, count));
}

// Also change SAD0 address to new address for Baby

/*************************************************
Function Description: Write to Battery babysitter register REG
Function Arguments:
    reg: register to write to
    pdata: array of data to write
    count: number of bytes
*************************************************/
esp_err_t wrBQ27441(uint8_t reg, uint8_t *pdata, uint8_t count)
{
    return (i2c_master_wr_slave(I2C_PORT, BQ72441_I2C_ADDRESS, reg, pdata, count));
}

/*************************************************
Function Description: Initialize battery babysitter
Function Arguments: N/A
*************************************************/
esp_err_t init_batt_baby()
{
    uint8_t deviceID[2];

    // Wire.begin(); // Initialize I2C master

    // deviceID = deviceType(); // Read deviceType from BQ27441

    deviceID[0] = 0x80;
    deviceID[1] = 0x00;

    while (true)
    {
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

    while ((val & (1 << 4)) == 0)
    {
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

    if (result == 0)
    {
        return result;
    }

    return result;
}
