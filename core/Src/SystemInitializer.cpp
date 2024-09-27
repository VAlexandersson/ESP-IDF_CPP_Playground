#include "SystemInitializer.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#include "esp_log.h"
#define LOG_TAG "SYS_INIT"

#include "config.h" // SSID and PASSWORD

//SystemInitializer::SystemInitializer(NVS::Nvs& nvs, WIFI::Wifi& wifi, SNTP::Sntp& sntp, Gpio::GpioOutput& led) : 
//    nvs{nvs}, wifi{wifi}, sntp{sntp}, led{led} {}

esp_err_t SystemInitializer::initialize() {
    esp_err_t status{ESP_OK};

    ESP_LOGI(LOG_TAG, "Initializing NVS");
    status = nvs.init();

    if (ESP_OK == status) {
        status = manage_wifi_credentrials();
        ESP_LOGI(LOG_TAG, "WiFi credentials set: %s", status == ESP_OK ? "OK" : "FAIL");
    }

    if (ESP_OK == status) status |= wifi.begin();

    while (WIFI::Wifi::get_state() != WIFI::Wifi::state_e::CONNECTED) {
        ESP_LOGI(LOG_TAG, "Waiting for WiFi to connect...");
        vTaskDelay(pdSECOND);
    }

    status |= sntp.init();

    status |= led.init();

    return status;
}

constexpr size_t SSID_LEN = 32;
constexpr size_t PASSWORD_LEN = 64;

esp_err_t SystemInitializer::manage_wifi_credentrials() {
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

    // Emulate we get it from provisioning service
    if (ESP_OK != status) {

        // We pass the ssid and password variables to the function which will populate them
        for (size_t i = 0; i < ssid_len; ++i) ssid[i] = SSID[i];
        for (size_t i = 0; i < password_len; ++i) password[i] = PASSWORD[i];
        if (memcmp(SSID, ssid, ssid_len) == 0 && memcmp(PASSWORD, password, password_len) == 0) {
            status = ESP_OK;
        } else {
            ESP_LOGI(LOG_TAG, "SSID and PASSWORD are not the same");
        }

        // We got the credentials, now we store them in NVS
        if (ESP_OK == status) {

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