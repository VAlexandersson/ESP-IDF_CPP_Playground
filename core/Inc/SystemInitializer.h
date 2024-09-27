#pragma once

#include "esp_err.h"

#include "../../application/Nvs32/Nvs32.h"
#include "Wifi.h"
#include "Sntp32.h"
#include "Gpio.h"

#define pdSECOND pdMS_TO_TICKS(1000)

class SystemInitializer {
public:
    SystemInitializer(NVS::Nvs& nvs, WIFI::Wifi& wifi, SNTP::Sntp& sntp, Gpio::GpioOutput& led) : 
        nvs{nvs}, wifi{wifi}, sntp{sntp}, led{led} {};
    esp_err_t initialize();

private:
    NVS::Nvs& nvs;
    WIFI::Wifi& wifi;
    SNTP::Sntp& sntp;
    Gpio::GpioOutput& led;

    esp_err_t manage_wifi_credentrials();
};