#ifndef SD_H
#define SD_H

#include "esp_log.h"
#include "esp_system.h"
//#include "esp_spi_flash.h"
#include "spi_flash_mmap.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#define PIN_NUM_MOSI 11
#define PIN_NUM_MISO 13
#define PIN_NUM_CLK 12
#define PIN_NUM_CS 10
#define MOUNT_POINT "/sdcard"

void init_sd();
void unmount_sd();
void read_sd(const char* path);

#endif
