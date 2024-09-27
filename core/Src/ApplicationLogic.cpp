#include "ApplicationLogic.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#include "esp_log.h"
#define LOG_TAG "APP_LOGIC"

ApplicationLogic::ApplicationLogic(Gpio::GpioOutput& led) : led{led} {}

void ApplicationLogic::run() {
    led.set(true);
    ESP_LOGI(LOG_TAG, "LED state: %d", led.state());
    vTaskDelay(pdSECOND);

    led.set(false);
    ESP_LOGI(LOG_TAG, "LED state: %d", led.state());
    vTaskDelay(pdSECOND);
}