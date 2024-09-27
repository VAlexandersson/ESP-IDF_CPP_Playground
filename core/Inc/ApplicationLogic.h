#pragma once

#include "freertos/FreeRTOS.h"
#include "Gpio.h"

#define pdSECOND pdMS_TO_TICKS(1000)

class ApplicationLogic {
public:
    ApplicationLogic(Gpio::GpioOutput& led);
    void run();

private:
    Gpio::GpioOutput& led;
};