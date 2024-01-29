
#include "Wifi.h"

namespace WIFI {

char                Wifi::mac_address_cstr[]{};
std::mutex          Wifi::init_mutex{};
Wifi::state_e       Wifi::_state{Wifi::state_e::NOT_INITIALIZED};
wifi_init_config_t  Wifi::wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
wifi_config_t       Wifi::wifi_config{};

Wifi::Wifi(void) {

    std::lock_guard<std::mutex> guard(init_mutex);

    if(!get_mac()[0]) { 
        if(ESP_OK != _get_mac()) 
            esp_restart(); 
    }
    
} // constructor Wifi

esp_err_t Wifi::_init(void) {

    std::lock_guard<std::mutex> guard(init_mutex);

    esp_err_t status{ESP_OK};

    if (state_e::NOT_INITIALIZED == _state) {
        status = esp_netif_init(); // Initialize the underlying TCP/IP stack 

        if (ESP_OK == status) {
            const esp_netif_t* const p_netif = esp_netif_create_default_wifi_sta(); // Create a default WiFi station with default config

            if (!p_netif) status = ESP_FAIL;
        }
        if (ESP_OK == status) {
            status = esp_wifi_init(&wifi_init_config);
        }
        if (ESP_OK == status) {
            status = esp_wifi_set_mode(WIFI_MODE_STA); // keep track of mode?
        }
        if (ESP_OK == status) {
            
            const size_t ssid_len_to_copy = 
                std::min( strlen(ssid), static_cast<size_t>(sizeof(wifi_config.sta.ssid)) );

            memcpy(wifi_config.sta.ssid, ssid, ssid_len_to_copy);

            const size_t password_len_to_copy = 
                std::min( strlen(password), static_cast<size_t>(sizeof(wifi_config.sta.password)) );

            memcpy(wifi_config.sta.password, password, password_len_to_copy);

            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
            wifi_config.sta.pmf_cfg.capable = true;
            wifi_config.sta.pmf_cfg.required = false;

            status = esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // keep track of mode?
        }


    } else if (state_e::ERROR == _state) {
        status = ESP_FAIL;
    }


    return status;
} // Wifi::init

esp_err_t Wifi::_get_mac(void) {

    uint8_t mac_byte_buffer[6]{};

    const esp_err_t status{esp_efuse_mac_get_default(mac_byte_buffer)};

    snprintf(mac_address_cstr, sizeof(mac_address_cstr), 
            "%02X%02X%02X%02X%02X%02X",
            mac_byte_buffer[0], mac_byte_buffer[1], 
            mac_byte_buffer[2], mac_byte_buffer[3], 
            mac_byte_buffer[4], mac_byte_buffer[5]);

    return status;

} // Wifi::_get_mac

} // namespace WIFI