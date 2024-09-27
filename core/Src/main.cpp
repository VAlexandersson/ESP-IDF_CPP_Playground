/*
#include "main.h"
#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#include "esp_log.h"
#define LOG_TAG "MAIN"

#include "config.h" // SSID and PASSWORD

static Main my_main;
NVS::Nvs Main::nvs{};

extern "C" void app_main(void) {

    ESP_LOGI(LOG_TAG, "Creating default event loop");    
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initializing NVS");    
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(my_main.setup());

    while (true) {
        my_main.loop();
    }
}

constexpr size_t SSID_LEN = 32;
constexpr size_t PASSWORD_LEN = 64;

esp_err_t Main::manage_wifi_credentrials() {
    char ssid[SSID_LEN]{};
    char password[PASSWORD_LEN]{};
    size_t ssid_len = std::min(strlen(SSID), SSID_LEN);
    size_t password_len = std::min(strlen(PASSWORD), PASSWORD_LEN);

    esp_err_t status{ESP_OK};
    if (ESP_OK != nvs.get_buffer("wifi_ssid", ssid, ssid_len) ||
        ESP_OK != nvs.get_buffer("wifi_password", password, password_len)) {
        ESP_LOGI(LOG_TAG, "WiFi credentials not found in NVS");
        status = ESP_FAIL;
    }

    // Emulate we get it from provisioning service, this code is only temporarly. IGNORE.
    if (ESP_OK != status) {
        
        // We pass the ssid and password variables to the function which will populate them
        for (size_t i = 0; i < ssid_len; ++i) ssid[i] = SSID[i];
        for (size_t i = 0; i < password_len; ++i) password[i] = PASSWORD[i];
        if(memcmp(SSID, ssid, ssid_len) == 0 && memcmp(PASSWORD, password, password_len) == 0) {
            status = ESP_OK;
        }
        else {
            ESP_LOGI(LOG_TAG, "SSID and PASSWORD are not the same");
        }


        // We got the credentials, now we store them in NVS
        if (ESP_OK == status){

            if (ESP_OK != nvs.set_buffer("wifi_ssid", ssid, ssid_len) ||
                ESP_OK != nvs.set_buffer("wifi_password", password, password_len)) {
                
                status = ESP_FAIL;

            }
            ESP_LOGI(LOG_TAG, "Setting new SSID and PASSWORD to NVS: %s", status == ESP_OK ? "OK" : "FAIL");
        }
    }


    if (ESP_OK == status) {
        ESP_LOGI(LOG_TAG, "Setting WiFi credentials");
        status = wifi.init(ssid, password);
    } 
    return status;
}


esp_err_t Main::setup(void) {
    esp_err_t status{ESP_OK};
    
    ESP_LOGI(LOG_TAG, "SETUP!");
    
    ESP_LOGI(LOG_TAG, "Initializing NVS");
    status = nvs.init();

    if (ESP_OK == status) {
        // Would it make more sense to pass the ssid and password as arguments and init the wifi from here?
        status = manage_wifi_credentrials();
        ESP_LOGI(LOG_TAG, "WiFi credentials set: %s", status == ESP_OK ? "OK" : "FAIL");
    }

    status |= led.init();

    if (ESP_OK == status) status |= wifi.begin();

    while (WIFI::Wifi::get_state() != WIFI::Wifi::state_e::CONNECTED) {
        ESP_LOGI(LOG_TAG, "Waiting for WiFi to connect...");
        vTaskDelay(pdSECOND); 
    }

    status |= sntp.init();

    return status;
}


void Main::loop(void) {


    //ESP_LOGI(LOG_TAG, "Hello world!");
    
    led.set(true);
    ESP_LOGI(LOG_TAG, "LED state: %d", led.state());
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    led.set(false);    
    ESP_LOGI(LOG_TAG, "LED state: %d", led.state());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "nvs_flash.h"

#include "Gpio.h"
#include "Wifi.h"
#include "Sntp32.h"
#include "../../application/Nvs32/Nvs32.h"

#include "ApplicationLogic.h"
#include "SystemInitializer.h"

#include "esp_log.h"
#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#define LOG_TAG "MAIN"

extern "C" void app_main(void) {

    ESP_LOGI(LOG_TAG, "Creating default event loop");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(LOG_TAG, "Initializing NVS");
    ESP_ERROR_CHECK(nvs_flash_init());

    NVS::Nvs nvs;
    WIFI::Wifi wifi;
    SNTP::Sntp& sntp = SNTP::Sntp::get_instance();
    Gpio::GpioOutput led{GPIO_NUM_2, false};

    SystemInitializer initializer(nvs, wifi, sntp, led);

    ESP_ERROR_CHECK(initializer.initialize());

    ApplicationLogic appLogic(led);

    while (true) {
        appLogic.run();
    }
}