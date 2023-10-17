#include "sd.h"

FILE *f = NULL;

/*************************************************
Function Description:
Function Arguments:
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

    printf("Host defined\n");

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        return;
    }

    printf("SPI bus defined\n");

    sdspi_device_config_t device = SDSPI_DEVICE_CONFIG_DEFAULT();

    device.host_id = host.slot;

    device.gpio_cs = PIN_NUM_CS;

    sdmmc_card_t *card;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    #ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
            .format_if_mount_failed = true,
    #else
            .format_if_mount_failed = false,
    #endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
            .max_files = 5,
            .allocation_unit_size = 16 * 1024
    };

    const char mount_point[] = MOUNT_POINT;

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &device, &mount_config, &card);

    printf("mounted\n");

    if (ret != ESP_OK) {
        return;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    printf("done\n");

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