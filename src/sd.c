#include "sd.h"

FILE *f = NULL;

/*************************************************
Function Description:
Function Arguments:
*************************************************/
void init_sd(){
    esp_err_t ret;

    const char* base_path = "/sdcard";
    
    sdmmc_host_t h;
    sdmmc_host_t* host = &h;
    //  = {
    //     .flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_8BIT,
    //     .slot = SDMMC_HOST_SLOT_0,
    //     .max_freq_khz = SDMMC_FREQ_DEFAULT,
    //     .io_voltage = 3.3,
    //     .command_timeout_ms = 0
    // };

    // sdmmc_card_t card = {
    //     .host = host,
    //     .max_freq_khz = SDMMC_FREQ_DEFAULT
    // };

    sdmmc_card_t c;
    sdmmc_card_t* card = &c;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    //slot_config.width = 1;

    ret = sdmmc_host_init();
    printf("return: %x\n", ret);
    ret = sdmmc_host_init_slot(SDMMC_HOST_SLOT_0, &slot_config);
    printf("return: %x\n", ret);
    
    host->init();

    ret = sdmmc_card_init(host, card);
    // printf("return: %x\n", ret);



    // testing

    // uint8_t dst [5 * (&card)->csd.sector_size];
    // ret = sdmmc_read_sectors(&card, &dst, 0, 5);

    // printf("return: %x\n", ret);

    // esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    //     .format_if_mount_failed = false,
    //     .max_files = 5
    // };

    // esp_vfs_fat_sdmmc_mount(base_path, &host, &slot_config, &mount_config);

}

/*************************************************
Function Description:
Function Arguments:
*************************************************/
void read_sd(){
    f = fopen("/sdcard/test.txt", "r");

    if(f == NULL){
        //failed
        return;
    }
    else{
        //read
    }

    fclose(f);
}

/*************************************************
Function Description:
Function Arguments:
*************************************************/
void unmount_sd(){
    // esp_vfs_fat_sdmmc_unmount();
}