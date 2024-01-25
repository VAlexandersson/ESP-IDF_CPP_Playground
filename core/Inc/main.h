#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "nvs_flash.h"

#include "Gpio.h"

 
class Main final {
public:
    esp_err_t setup(void);
    void loop(void);

    Gpio::GpioOutput led{GPIO_NUM_2, false};
};