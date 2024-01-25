#include "main.h"
#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#include "esp_log.h"
#define LOG_TAG "MAIN"

static Main my_main;

extern "C" void app_main(void) {

    ESP_LOGI(LOG_TAG, "Creating defaiult event loop");    
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initializing NVS");    
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(my_main.setup());

    while (true) {
        my_main.loop();
    }
}

esp_err_t Main::setup(void) {
    esp_err_t status{ESP_OK};
    ESP_LOGI(LOG_TAG, "SETUP!");
    status |= led.init();
    return status;
}


void Main::loop(void) {


    ESP_LOGI(LOG_TAG, "Hello world!");
    
    led.set(true);
    ESP_LOGI(LOG_TAG, "LED state: %d", led.state());
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    led.set(false);    
    ESP_LOGI(LOG_TAG, "LED state: %d", led.state());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}