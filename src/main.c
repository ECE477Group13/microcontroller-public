#include "main.h"
#include "i2smicro.h"
#include "sd.h"

void app_main() {    

    printf("waiting");
    for (int i = 0; i < 50000; i++) {
        printf(".");
    }
    printf("done waiting");
    printf("\n");

    /* SD Card stuff */
    init_sd();
    init_i2s_tx(); 

    const char* path = "/sdcard/wave1.wav";
    //read_sd(path);
    play_wav(path);

    unmount_sd();
    destroy_i2s_tx();

    printf("unmounted\n");

}