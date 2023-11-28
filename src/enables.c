
#include "enables.h"

void init_enables() {
    gpio_set_direction(GPS_EN, GPIO_MODE_OUTPUT);
    gpio_set_direction(BOOST_EN, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER_EN, GPIO_MODE_OUTPUT);

    gpio_set_pull_mode(BOOST_EN, GPIO_PULLDOWN_ONLY);

    gpio_set_level(GPS_EN, 0);
    gpio_set_level(BOOST_EN, 0);
    gpio_set_level(BUZZER_EN, 0);
}


