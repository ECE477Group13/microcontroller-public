#include "sd.h"

FILE *f = NULL;

/*************************************************
Function Description:
Function Arguments:
*************************************************/
void init_sd(){
    esp_err_t ret;

    const char* base_path = "/sdcard";
    
    sdmmc_host_t host = SDMMC_HOST_DEFAULT;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT;
    slot_config.width = 1;

    ret = sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);

    sdmmc_card_t* card;
    sdmmc_card_init(&host, &card);

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5
    };

    esp_vfs_fat_sdmmc_mount(base_path, &host, &slot_config, &mount_config);

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
    esp_vfs_fat_sdmmc_unmount();
}