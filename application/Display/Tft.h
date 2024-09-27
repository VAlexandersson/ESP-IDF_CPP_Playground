#pragma once

#include <cstring>

#include "esp_err.h"
#include "driver/spi_master.h"
#include "../Gpio/Gpio.h"

namespace Display {

class Tft {
public:
    // Constructor (for now, we'll hardcode the SPI and GPIO configuration)
    Tft(); 

    esp_err_t init(void);
    void clear(const uint16_t color);

private:
    spi_device_handle_t _spi;
    Gpio::GpioOutput _dc;
    Gpio::GpioOutput _rst;

    void sendCommand(const uint8_t command);
    void sendData(const uint8_t data);
};

} // namespace Display