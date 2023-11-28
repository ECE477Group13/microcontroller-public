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
Function Description: Initializes battery babysitter
Function Arguments: None
*************************************************/
esp_err_t init_batt_baby(){
    
    esp_err_t result1;
    esp_err_t result2;

    // from 3.1 Data Memory Parameter Update Example

    uint8_t data [2];
    esp_err_t result;

    data[0] = 0x00;
    data[1] = 0x80;
    result = wrBQ27441(0x00, data, 2);
    printf("%x \n", result);
    result = wrBQ27441(0x00, data, 2);
    printf("%x \n", result);
    
    data[0] = 0x13;
    data[1] = 0x00;
    result = wrBQ27441(0x00, data, 2);
    printf("%x \n", result);

    uint8_t flags_register = 0x0;
    while ((flags_register & (1<<4)) == 0) {
        result = rdBQ27441(0x06, &flags_register, 1);
        printf("%x flg: %x\n", result, flags_register);
    }
    
    data[0] = 0x00;
    result = wrBQ27441(0x61, data, 1);
    printf("%x \n", result);

    data[0] = 0x52;
    result = wrBQ27441(0x3E, data, 1);
    printf("%x \n", result);

    data[0] = 0x00;
    result = wrBQ27441(0x3F, data, 1);
    printf("%x \n", result);
    
    uint8_t old_checksum = 0x0;
    result = rdBQ27441(0x60, &old_checksum, 1);
    printf("%x oldcheck: %x\n", result, old_checksum);

    uint8_t old_descap_msb = 0x0;
    result = rdBQ27441(0x4A, &old_descap_msb, 1);
    printf("%x old descap msb: %x\n", result, old_descap_msb);

    uint8_t old_descap_lsb = 0x0;
    result = rdBQ27441(0x4B, &old_descap_lsb, 1);
    printf("%x old descap lsb: %x\n", result, old_descap_lsb);

    data[0] = 2000 / 256;
    data[1] = 2000 % 256;
    result = wrBQ27441(0x4A, &(data[0]), 1);
    printf("%x\n", result);
    result = wrBQ27441(0x4B, &(data[1]), 1);
    printf("%x\n", result);

    uint8_t tmp = (255 - old_checksum - old_descap_msb - old_descap_lsb) % 256;
    uint8_t new_checksum = 255 - ((tmp + data[0] + data[1]) % 256);

    result = wrBQ27441(0x60, &new_checksum, 1);
    printf("%x new checksum: %x\n", result, new_checksum);
    
    data[0] = 0x42;
    data[1] = 0x00;
    result = wrBQ27441(0x00, data, 2);
    
    flags_register = 0x0;
    while ((flags_register & (1<<4)) != 0) {
        result = rdBQ27441(0x06, &flags_register, 1);
        printf("%x flg: %x\n", result, flags_register);
    }

    // end

    uint8_t lower;
    uint8_t higher;

    result1 = rdBQ27441(0x08, &lower, 1);
    result2 = rdBQ27441(0x09, &higher, 1);
    printf("%x %x  nominal: %d \n", result1, result2, (higher << 8) | lower);

    result1 = rdBQ27441(0x0A, &lower, 1);
    result2 = rdBQ27441(0x0B, &higher, 1);
    printf("%x %x  full avl: %d \n", result1, result2, (higher << 8) | lower);

    result1 = rdBQ27441(0x0C, &lower, 1);
    result2 = rdBQ27441(0x0D, &higher, 1);
    printf("%x %x  rem: %d \n", result1, result2, (higher << 8) | lower);

    return ESP_OK;

}


/*************************************************
Function Description: Retrieve percentage
Function Arguments: None
*************************************************/
uint8_t get_batt_percent() {

    uint8_t lower;
    uint8_t higher;

    rdBQ27441(0x0A, &lower, 1);
    rdBQ27441(0x0B, &higher, 1);
    uint16_t full_avl = (higher << 8) | lower;

    rdBQ27441(0x0C, &lower, 1);
    rdBQ27441(0x0D, &higher, 1);
    uint16_t rem = (higher << 8) | lower;

    return 100 * rem / full_avl;
}

/*************************************************
Function Description: Retrieve current
Function Arguments: None
*************************************************/
int16_t get_batt_current() {

    uint8_t lower;
    uint8_t higher;

    rdBQ27441(0x10, &lower, 1);
    rdBQ27441(0x11, &higher, 1);
    int16_t full_val = (higher << 8) | lower;

    return full_val;
}