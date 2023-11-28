/*************************************************
 BoardLock - Team 13 - 2023

 File Description: GPS functions
*************************************************/
#include "gps.h"
#include "rgb.h" // TEMPORARY

static int32_t latitude = 0;
static int32_t longitude = 0;
static int64_t last_pos_time = 0;

/*************************************************
Function Description: Retrieve latitude
Function Arguments: None
*************************************************/
int32_t get_lat()
{
    // latitude = latitude - 1;
    return latitude;
}

/*************************************************
Function Description: Retrieve longitude
Function Arguments: None
*************************************************/
int32_t get_long()
{
    // longitude = longitude + 1;
    return longitude;
}

/*************************************************
Function Description: Retrieve last position time
Function Arguments: None
*************************************************/
int64_t get_last_pos_time()
{
    return last_pos_time;
}

/*************************************************
Function Description: Read from GPS register reg
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
Function Description: prints out the gps data stream, for debugging purposes
Function Arguments: None
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

/*************************************************
Function Description: Send UBX protocol message to GPS 
Function Arguments:
- class: byte for class of message
- id: byte for id of message
- len: length of payload
- payload: array of bytes to send as payload
*************************************************/
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

/*************************************************
Function Description: requests lat and long
Function Arguments: None
*************************************************/
esp_err_t send_location_request()
{
    esp_err_t send;
    
    send = ubx_send_msg(0x01, 0x02, 0, NULL);
    printf("GPS error: %x\n", send);
    if (send != ESP_OK) return ESP_FAIL;
    return ESP_OK;
}

/*************************************************
Function Description: retrieve latitude and longitude from GPS
Function Arguments: None
*************************************************/
esp_err_t update_location()
{
    // uint8_t payload = 0x00; // TEMPORARY
    // send = ubx_send_msg(0x06, 0x00, 1, &payload); // TEMPORARY
    
    int32_t new_long = 0;
    int32_t new_lat = 0;
    bool isNew = false;
    uint8_t data_byte = 0;

    int i = 0;

    int tries = 0;
    int zero_tries = 0;
    while (tries < 5 && zero_tries < 200) {
        rdSAMM8Q(SAMM8Q_DATA_STREAM, &data_byte, 1);
        if (data_byte == 0xb5) printf("\n");
        printf("%x ", data_byte);
        if ((data_byte == 0xb5 && i == 0) ||
            (data_byte == 0x62 && i == 1) ||
            (data_byte == 0x01 && i == 2) ||
            (data_byte == 0x02 && i == 3) ||
            (i >= 4)) {

            switch(i) {
                case 10: new_long |= data_byte;         break;
                case 11: new_long |= data_byte << 8;    break;
                case 12: new_long |= data_byte << 16;   break;
                case 13: new_long |= data_byte << 24;   break;
                case 14: new_lat |= data_byte;          break;
                case 15: new_lat |= data_byte << 8;     break;
                case 16: new_lat |= data_byte << 16;    break;
                case 17: new_lat |= data_byte << 24;    break;
                default:                                break;
            }

            if (i == 17) {
                if (new_long != 0 && new_lat != 0) {
                    longitude = new_long;
                    latitude = new_lat;
                    last_pos_time = esp_timer_get_time();
                    isNew = true;
                } else {
                    isNew = false;
                }
                i = 0;
            } else {
                i++;
            }
        } else {
            i = 0;
        }

        if (data_byte == 0xFF) tries ++;
        if (data_byte == 0x00) zero_tries ++;
    }

    if (isNew) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

// timer related functions

#define SEC             1000000

static bool isPolling = false;

esp_timer_handle_t gps_wait_timer_handler;  
esp_timer_handle_t gps_poll_timer_handler;  
esp_timer_handle_t gps_stop_timer_handler;  

void gps_wait_timer_cb(void *param) {
/*************************
 description: callback function that timer interrupt calls signal warning period over
 arguments: 
 - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
 return: N/A
**************************/
    update_location();
    esp_timer_stop(gps_wait_timer_handler);
}

void gps_poll_timer_cb(void *param) {
/*************************
 description: callback function that timer interrupt calls signal warning period over
 arguments: 
 - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
 return: N/A
**************************/
    send_location_request();
    esp_timer_stop(gps_wait_timer_handler);
    ESP_ERROR_CHECK(esp_timer_start_once(gps_wait_timer_handler, 1 * SEC));
}

void gps_stop_timer_cb(void *param) {
/*************************
 description: callback function that timer interrupt calls signal warning period over
 arguments: 
 - void *param: DO NOT TOUCH (this is the necessary config but will not have anything)
 return: N/A
**************************/
    esp_timer_stop(gps_poll_timer_handler);
    esp_timer_stop(gps_stop_timer_handler);
    gpio_set_level(GPS_EN, 0);
    isPolling = false;
}

void init_gps() {
    
    //disabling nmea messages

    uint8_t config [20];
    config[0] = 0;
    config[1] = 0;
    config[2] = 0;
    config[3] = 0;
    config[4] = 0x84;
    config[5] = 0x0;
    config[6] = 0x0;
    config[7] = 0x0;
    config[8] = 0x0;
    config[9] = 0x0;
    config[10] = 0x0;
    config[11] = 0x0;
    config[12] = 0x7;
    config[13] = 0x0;
    config[14] = 0x1;
    config[15] = 0x0;
    config[16] = 0x0;
    config[17] = 0x0;
    config[18] = 0x0;
    config[19] = 0x0;
    ubx_send_msg(0x06, 0x00, 20, config);
    
    // timers
    
    const esp_timer_create_args_t timer_wait_cfg = {
        .callback = &gps_wait_timer_cb,
        .name = "gps poll timer"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timer_wait_cfg, &gps_wait_timer_handler));

    const esp_timer_create_args_t timer_poll_cfg = {
        .callback = &gps_poll_timer_cb,
        .name = "gps poll timer"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timer_poll_cfg, &gps_poll_timer_handler));

    const esp_timer_create_args_t timer_stop_cfg = {
        .callback = &gps_stop_timer_cb,
        .name = "gps stop timer"
    };
    
    ESP_ERROR_CHECK(esp_timer_create(&timer_stop_cfg, &gps_stop_timer_handler));
}

void gps_start_polling() {
    gpio_set_level(GPS_EN, 1);
    
    if (isPolling) {
        esp_timer_stop(gps_stop_timer_handler);
        ESP_ERROR_CHECK(esp_timer_start_once(gps_stop_timer_handler, 60 * SEC));
    } else {
        esp_timer_stop(gps_poll_timer_handler);
        esp_timer_stop(gps_stop_timer_handler);
        ESP_ERROR_CHECK(esp_timer_start_periodic(gps_poll_timer_handler, 5 * SEC));
        ESP_ERROR_CHECK(esp_timer_start_once(gps_stop_timer_handler, 60 * SEC));
        isPolling = true;
    }
}