#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define pdSECOND pdMS_TO_TICKS(1000)

#include "esp_event.h"
#include "nvs_flash.h"

#include "Gpio.h"
#include "Wifi.h"
#include "SntpTime.h"
#include "../../application/Nvs32/Nvs32.h"
 
class Main final {
public:
    Main(void) : sntp{SNTP::Sntp::get_instance()} { }

    esp_err_t setup(void);
    void loop(void);

    esp_err_t manage_wifi_credentrials();


    Gpio::GpioOutput led{GPIO_NUM_2, false};
    WIFI::Wifi wifi;
    SNTP::Sntp& sntp;

    static NVS::Nvs nvs;
};