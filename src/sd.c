#include "sd.h"

#define TAG "SD"

FILE *f = NULL;
const char mount_point[] = MOUNT_POINT;
sdmmc_card_t *card;

/*************************************************
Function Description: 
    Initialize SD card
Function Arguments:
    N/A
*************************************************/
void init_sd(){
    esp_err_t ret;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        return;
    }

    sdspi_device_config_t device = SDSPI_DEVICE_CONFIG_DEFAULT();

    device.host_id = host.slot;

    device.gpio_cs = PIN_NUM_CS;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    #ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
            .format_if_mount_failed = true,
    #else
            .format_if_mount_failed = false,
    #endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
            .max_files = 5,
            .allocation_unit_size = 16 * 1024
    };

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &device, &mount_config, &card);

    if (ret != ESP_OK) {
        return;
    }

    /* Card has been initialized, print its properties */
    // sdmmc_card_print_info(stdout, card);

    ESP_LOGI(TAG, "SD Card Initialized.");

}

/*************************************************
Function Description:
    Read data from sd_card in binary
    ONLY USED FOR DEBUGGING
Function Arguments: 
    path - Path of file to read from. Need to 
           include mount point before the file
           name. Ex) "/sdcard/wave1.wav"
*************************************************/
void read_sd(const char* path){
    f = fopen(path, "rb");

    if(f == NULL){
        ESP_LOGI(TAG, "FAILED to open path on SD.");
        return;
    }
    else{
        char line[64];
        fgets(line, sizeof(line), f);
        fclose(f);
    }

}

/*************************************************
Function Description:
    Unmount SD Card
Function Arguments:
    N/A
*************************************************/
void unmount_sd(){
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    //spi_bus_free(host.slot);

    ESP_LOGI(TAG, "SD Card Unmounted.");
}