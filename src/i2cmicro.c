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
static esp_err_t i2c_master_rd_slave(i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t *data_rd, size_t size)
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
static esp_err_t i2c_master_wr_slave(i2c_port_t i2c_port, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t *data_wr, size_t size)
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
Function Description:
    Read from GPS register reg
Function Arguments:
    reg: register that is being read
    pdata: array of data to store read bytes in
    count: number of bytes
*************************************************/
esp_err_t rdSAMM8Q(uint8_t reg, uint8_t *pdata, uint8_t count)
{
    return (i2c_master_rd_slave(I2C_PORT, SAMM8Q_I2C_ADDR, reg, pdata, count));
}

/*************************************************
Function Description: Write to GPS register reg
Function Arguments:
*************************************************/
esp_err_t wrSAMM8Q(uint8_t reg, uint8_t *pdata, uint8_t count)
{
    return (i2c_master_wr_slave(I2C_PORT, SAMM8Q_I2C_ADDR, reg, pdata, count));
}

/*************************************************
Function Description:
Function Arguments:
*************************************************/
esp_err_t init_imu()
{
    // section 4.1 of LSM6DSO32 appl note
    esp_err_t err;

    uint8_t value = 01;
    err = wrLSM6DS(LSM6DS_INT1_CTRL, &value, 1);
    if (err != 0)
        return err;

    value = 0x60;
    err = wrLSM6DS(LSM6DS_CTRL1_XL, &value, 1);
    return err;
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


// /*************************************************
// Function Description:
// Function Arguments:
// *************************************************/
// esp_err_t init_gps()
// {
//     esp_err_t err;
// }

/*************************************************
Function Description:
Function Arguments:
*************************************************/
uint8_t print_gps_data_stream()
{
    esp_err_t err;

    uint8_t num_bytes_avl_hi;
    uint8_t num_bytes_avl_lo;
    err = rdSAMM8Q(SAMM8Q_NUM_BYTES_AVL_HI, &num_bytes_avl_hi, 1);
    if (err != 0) printf("hi err: %x\n", err);
    err = rdSAMM8Q(SAMM8Q_NUM_BYTES_AVL_LO, &num_bytes_avl_lo, 1);
    if (err != 0) printf("lo err: %x\n", err);

    uint16_t num_bytes_avl = (num_bytes_avl_hi << 8) | num_bytes_avl_lo;
    printf("num_bytes_avl: %d\n", num_bytes_avl);

    if (num_bytes_avl > 0) {
        
        
        uint8_t data_byte = 0;
        uint8_t try = 0;

        while (try <= 5 || data_byte != 0xFF) {
            err = rdSAMM8Q(SAMM8Q_DATA_STREAM, &data_byte, 1);
            if (err != 0) printf("rd err: %x\n", err);
            if (!err) {
                if (data_byte == 0xb5) printf("\n");
                printf("%x ", data_byte);
            }
            if (data_byte == 0xFF) try ++;
        }
        printf("\n");

        return 1;
    }

    return 0;
}

esp_err_t ubx_send_msg(uint8_t class, uint8_t id, uint16_t len, uint8_t* payload) {

	uint8_t ck_a = 0;
	uint8_t ck_b = 0;

	const uint8_t sync_char_1 = 0xB5;
	const uint8_t sync_char_2 = 0x62;

	// create command queue
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // queue a start command
    i2c_master_start(cmd);

    	// send device address
    i2c_master_write_byte(cmd, (SAMM8Q_I2C_ADDR << 1) | WR_BIT, ACK_CHECK_EN);

	// send sync char 1 
	i2c_master_write_byte(cmd, sync_char_1, ACK_CHECK_EN);

	// send sync char 2
	i2c_master_write_byte(cmd, sync_char_2, ACK_CHECK_EN);

	// send class
	i2c_master_write_byte(cmd, class, ACK_CHECK_EN);
	ck_a += class; ck_b += ck_a;	

	// send id
	i2c_master_write_byte(cmd, id, ACK_CHECK_EN);
	ck_a += id; ck_b += ck_a;

	// send len
    i2c_master_write_byte(cmd, (len) & 0xFF, ACK_CHECK_EN);
	ck_a += (len) & 0xFF; ck_b += ck_a;
    i2c_master_write_byte(cmd, (len >> 8) & 0xFF, ACK_CHECK_EN);
    ck_a += (len >> 8) & 0xFF; ck_b += ck_a;

    printf("ck_a, ck_b calculated: %x, %x\n", ck_a, ck_b);

    // send CK_A
    i2c_master_write_byte(cmd, ck_a, ACK_CHECK_EN);

    // send CK_B
    i2c_master_write_byte(cmd, ck_b, ACK_CHECK_EN);

    // stop
    i2c_master_stop(cmd);

    // send
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, TICK_RATE);

    i2c_cmd_link_delete(cmd);

    return ret;
}

int long_loop2() {
    volatile int j = 0;
    ESP_LOGI("i2cmicro", "Start wait");
    for (int i = 0; i < 1; i++) {
        for(int k = 0; k < 10000000; k++) { // 10000000
            j = (i | j) - 1 + k;
        }
    }
    ESP_LOGI("i2cmicro", "End wait");

    // for (int i = 0; i < 5000; i++) {
    //     ESP_LOGI(TAG, ".");
    // }

    return j;
}

void read_gps_port_config()
{
    esp_err_t err;

    uint8_t payload;
    
    // payload = 0x00;
    // err = ubx_send_msg(0x06, 0x00, 1, &payload);
    // if (err != 0) printf("msg err: %x\n", err);
    
    // uint8_t config [20];
    // config[0] = 1;
    // config[1] = 0;
    // config[2] = 0;
    // config[3] = 0;
    // config[4] = 0xc0;
    // config[5] = 0x8;
    // config[6] = 0x0;
    // config[7] = 0x0;
    // config[8] = 0x80;
    // config[9] = 0x25;
    // config[10] = 0x0;
    // config[11] = 0x0;
    // config[12] = 0x0;
    // config[13] = 0x0;
    // config[14] = 0x0;
    // config[15] = 0x0;
    // config[16] = 0x0;
    // config[17] = 0x0;
    // config[18] = 0x0;
    // config[19] = 0x0;
    // err = ubx_send_msg(0x06, 0x00, 20, config);
    // if (err != 0) printf("msg err: %x\n", err);

    // long_loop2();

    // payload = 0x01;
    // err = ubx_send_msg(0x06, 0x00, 1, &payload);
    // if (err != 0) printf("msg err: %x\n", err);
    
    // payload = 0;
    // err = ubx_send_msg(0x06, 0x06, 52, &payload, 1);
    // if (err != 0) printf("msg err: %x\n", err);
    
    // UBX-NAV-AOPSTATUS, seems to return something good.
    // status field can be monitored for 0 to see if 
    // reciever should be shut down, see pg 357. 
    // payload = 0;
    // err = ubx_send_msg(0x01, 0x60, 0, &payload, 0);
    // if (err != 0) printf("msg err: %x\n", err);

    // works it seems
    // payload = 0;
    // err = ubx_send_msg(0x01, 0x39, 0, &payload, 0);
    // if (err != 0) printf("msg err: %x\n", err);

    // this doesn't work because version isn't supported
    // payload = 0;
    // err = ubx_send_msg(0x01, 0x14, 0, &payload, 0);
    // if (err != 0) printf("msg err: %x\n", err);

    // payload = 0;
    err = ubx_send_msg(0x01, 0x02, 0, &payload);
    // if (err != 0) printf("msg err: %x\n", err);

    
    // payload = 0x02;
    // err = ubx_send_msg(0x06, 0x00, 1, &payload);
    // if (err != 0) printf("msg err: %x\n", err);
    
    // payload = 0x03;
    // err = ubx_send_msg(0x06, 0x00, 1, &payload);
    // if (err != 0) printf("msg err: %x\n", err);

    // err = ubx_send_msg(0x06, 0x06, 0, &payload);
    // if (err != 0) printf("msg err2: %x\n", err);
}

esp_err_t print_gps_coordinates()
{
    esp_err_t err;
    
    ESP_ERROR_CHECK(ubx_send_msg(0x01, 0x02, 0, NULL));
        
    int32_t longitude = 0;
    int32_t latitude = 0;

    uint8_t data_byte = 0;

    int i = 0;

    int tries = 0;
    while (tries < 5) {
        err = rdSAMM8Q(SAMM8Q_DATA_STREAM, &data_byte, 1);
        if (err != 0) printf("rd err: %x\n", err);
        if (!err) {
                 if (data_byte == 0xb5 && i == 0) i ++;
            else if (data_byte == 0x62 && i == 1) i ++;
            else if (data_byte == 0x01 && i == 2) i ++;
            else if (data_byte == 0x02 && i == 3) i ++;
            else if (i >= 4) {
                if (i == 10) longitude |= data_byte;
                if (i == 11) longitude |= data_byte << 8;
                if (i == 12) longitude |= data_byte << 16;
                if (i == 13) longitude |= data_byte << 24;
                if (i == 14) latitude |= data_byte;
                if (i == 15) latitude |= data_byte << 8;
                if (i == 16) latitude |= data_byte << 16;
                if (i == 17) latitude |= data_byte << 24;
                if (i == 18) i = -1;
                i++;
            }
            else i = 0;
        }



        i ++;
        if (data_byte == 0xFF) tries ++;
    }
    printf("%ld %ld\n", latitude, longitude);

    return ESP_OK;
}