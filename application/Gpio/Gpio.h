#pragma once

#include "driver/gpio.h"

namespace Gpio {

class GpioBase {
protected:
    const gpio_num_t _pin;
    const gpio_config_t _config;
    const bool _inverted_logic = false;

public:
    constexpr GpioBase(const gpio_num_t pin, const gpio_config_t& config, const bool inverted = false) :
        _pin{pin},
        _config{config},
        _inverted_logic{inverted}
    {  }

    virtual bool state(void) = 0;
    virtual esp_err_t set(const bool state) = 0;

    [[nodiscard]] esp_err_t init(void);
}; // class GpioBase


class GpioOutput : public GpioBase {
private:
    bool _state = false;

public:
    constexpr GpioOutput(const gpio_num_t pin, const bool inverted = false) : 
        GpioBase(
            pin, 
            gpio_config_t {
                .pin_bit_mask = 1ULL << pin,
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = GPIO_PULLDOWN_ENABLE,
                .intr_type = GPIO_INTR_DISABLE
            },
            inverted
        ) { }


    [[nodiscard]] esp_err_t init(void);

    esp_err_t set(const bool state);   

    //esp_err_t toggle(void); 

    bool state(void) {return _state;};

}; // class GpioOutput

class GpioInput {
//     gpio_num_t _pin;
//     const bool _inverted_logic = false;

// public:
//     esp_err_t init(void);
//     bool state(void);
//     const gpio_num_t _pin;
};

} // namespace Gpio
