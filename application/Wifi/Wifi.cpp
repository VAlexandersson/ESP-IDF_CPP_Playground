
#include "Wifi.h"
namespace WIFI {

char                Wifi::mac_address_cstr[]{};
std::mutex          Wifi::init_mutex{};
std::mutex          Wifi::connect_mutex{};
std::mutex          Wifi::state_mutex{};

Wifi::state_e       Wifi::_state{Wifi::state_e::NOT_INITIALIZED};
wifi_init_config_t  Wifi::wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
wifi_config_t       Wifi::wifi_config{};

NVS::Nvs            Wifi::nvs{};

Wifi::Wifi(void) {


    std::lock_guard<std::mutex> guard(init_mutex);

    if(!get_mac()[0]) { 
        if(ESP_OK != _get_mac()) 
            esp_restart(); 
    }
    
} // constructor Wifi



void Wifi::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {

    if(WIFI_EVENT == event_base) {
        ESP_LOGI(_log_tag, "%s:%d WIFI_EVENT received", __func__, __LINE__);
        return wifi_event_handler(arg, event_base, event_id, event_data);
    } 
    else if (IP_EVENT == event_base) {
        ESP_LOGI(_log_tag, "%s:%d IP_EVENT received", __func__, __LINE__);
        return ip_event_handler(arg, event_base, event_id, event_data);
    } 
    else {
        ESP_LOGE(_log_tag, "%s:%d Unexpected event: %s", __func__, __LINE__, event_base);
    }
}

void Wifi::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if(WIFI_EVENT == event_base) {
        const wifi_event_t event_type{static_cast<wifi_event_t>(event_id)};
        
        ESP_LOGI(_log_tag, "%s:%d Event ID %ld", __func__, __LINE__, event_id);

        switch(event_type) {
        case WIFI_EVENT_STA_START: {
            ESP_LOGI(_log_tag, "%s:%d STA_START, waiting for state_mutex", __func__, __LINE__);
            std::lock_guard<std::mutex> guard(state_mutex); 
            _state = state_e::READY_TO_CONNECT;
            ESP_LOGI(_log_tag, "%s:%d READY_TO_CONNECT", __func__, __LINE__);
            break;
        }
        case WIFI_EVENT_STA_CONNECTED: {
            ESP_LOGI(_log_tag, "%s:%d STA_CONNECTED, waiting for state_mutex", __func__, __LINE__);

            std::lock_guard<std::mutex> guard(state_mutex); 
            _state = state_e::WAITING_FOR_IP;
            ESP_LOGI(_log_tag, "%s:%d WAITING_FOR_IP", __func__, __LINE__);
            break;
        }
        default:
            // stop and disco... etc.
            ESP_LOGW(_log_tag, "%s:%d Default switch case (%ld)", __func__, __LINE__, event_id);
            break;
        }
    }
}

void Wifi::ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if(IP_EVENT == event_base) {
        const ip_event_t event_type{static_cast<ip_event_t>(event_id)};
        
        ESP_LOGI(_log_tag, "%s:%d Event ID %ld", __func__, __LINE__, event_id);

        switch(event_type) {
        case IP_EVENT_STA_GOT_IP: {
            ESP_LOGI(_log_tag, "%s:%d Got IP, waiting for state_mutex", __func__, __LINE__);
            std::lock_guard<std::mutex> guard(state_mutex);
            _state = state_e::CONNECTED;
            ESP_LOGI(_log_tag, "%s:%d Got IP, CONNECTED", __func__, __LINE__);
            break;
        }
        case IP_EVENT_STA_LOST_IP:{
            ESP_LOGW(_log_tag, "%s:%d Lost IP, waiting for state_mutex", __func__, __LINE__);
            std::lock_guard<std::mutex> guard(state_mutex);
            _state = state_e::WAITING_FOR_IP;
            ESP_LOGI(_log_tag, "%s:%d WAITING_FOR_IP", __func__, __LINE__);
            break;
        }
        default:
            // TODO: IP6.
            ESP_LOGW(_log_tag, "%s:%d Default switch case (%ld)", __func__, __LINE__, event_id);
            break;
        }
    }
}

esp_err_t Wifi::init(void) {
    return _init();
}

esp_err_t Wifi::begin(void) {
    ESP_LOGI(_log_tag, "%s:%d Waiting for connect_mutex", __func__, __LINE__);
    std::lock_guard<std::mutex> connect_guard(connect_mutex);
    esp_err_t status{ESP_OK};
    
    ESP_LOGI(_log_tag, "%s:%d Waiting for state_mutex", __func__, __LINE__);
    std::lock_guard<std::mutex> state_guard(state_mutex);
    switch(_state) {
    case state_e::READY_TO_CONNECT:
        ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_connect", __func__, __LINE__);
        status = esp_wifi_connect();
        ESP_LOGI(_log_tag, "%s:%d esp_wifi_connect:%s", __func__, __LINE__, esp_err_to_name(status));
        
        if (ESP_OK == status) 
            _state = state_e::CONNECTING;
        break;
    case state_e::CONNECTING:
    case state_e::WAITING_FOR_IP:
    case state_e::CONNECTED:
        break;
    case state_e::NOT_INITIALIZED:
    case state_e::INITIALIZED:
    case state_e::DISCONNECTED:
    case state_e::ERROR:
        ESP_LOGE(_log_tag, "%s:%d ERROR state", __func__, __LINE__);
        status = ESP_FAIL;
        break;
    }
    return status;
}

esp_err_t Wifi::_init(void) {

    ESP_LOGI(_log_tag, "%s:%d Waiting for init_mutex", __func__, __LINE__);
    std::lock_guard<std::mutex> init_guard(init_mutex);

    esp_err_t status{ESP_OK};
    ESP_LOGI(_log_tag, "%s:%d Waiting for state_mutex", __func__, __LINE__);
    std::lock_guard<std::mutex> state_guard(state_mutex);
    if (state_e::NOT_INITIALIZED == _state) {

        ESP_LOGI(_log_tag, "%s:%d Calling esp_netif_init", __func__, __LINE__);
        status = esp_netif_init(); // Initialize the underlying TCP/IP stack 
        ESP_LOGI(_log_tag, "%s:%d esp_netif_init:%s", __func__, __LINE__, esp_err_to_name(status));

        if (ESP_OK == status) {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_netif_create_default_wifi_sta", __func__, __LINE__);
            const esp_netif_t* const p_netif = esp_netif_create_default_wifi_sta(); // Create a default WiFi station with default config
            ESP_LOGI(_log_tag, "%s:%d esp_netif_create_default_wifi_sta:%p", __func__, __LINE__, p_netif);

            if (!p_netif) status = ESP_FAIL;
        }
        if (ESP_OK == status) {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_init", __func__, __LINE__);
            status = esp_wifi_init(&wifi_init_config);
            ESP_LOGI(_log_tag, "%s:%d esp_wifi_init:%s", __func__, __LINE__,
                                                        esp_err_to_name(status));
        }
        if (ESP_OK == status) {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_event_handler_instance_register", __func__, __LINE__);
            status = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, nullptr, nullptr);
            ESP_LOGI(_log_tag, "%s:%d esp_event_handler_instance_register:%s", __func__, __LINE__, esp_err_to_name(status));
        }
        if (ESP_OK == status) {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_event_handler_instance_register", __func__, __LINE__);
            status = esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler, nullptr, nullptr);
            ESP_LOGI(_log_tag, "%s:%d esp_event_handler_instance_register:%s", __func__, __LINE__,
                                                        esp_err_to_name(status));
        }
        if (ESP_OK == status) {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_mode", __func__, __LINE__);
            status = esp_wifi_set_mode(WIFI_MODE_STA); // keep track of mode?
            ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_mode:%s", __func__, __LINE__,
                                                        esp_err_to_name(status));
        }
        if (ESP_OK == status) {
            
            const size_t ssid_len_to_copy = 
                std::min( strlen(SSID), static_cast<size_t>(sizeof(wifi_config.sta.ssid)) );

            memcpy(wifi_config.sta.ssid, SSID, ssid_len_to_copy);

            const size_t password_len_to_copy = 
                std::min( strlen(PASSWORD), static_cast<size_t>(sizeof(wifi_config.sta.password)) );

            memcpy(wifi_config.sta.password, PASSWORD, password_len_to_copy);

            wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
            wifi_config.sta.pmf_cfg.capable = true;
            wifi_config.sta.pmf_cfg.required = false;
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_set_config", __func__, __LINE__);
            status = esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // keep track of mode?
            ESP_LOGI(_log_tag, "%s:%d esp_wifi_set_config:%s", __func__, __LINE__, esp_err_to_name(status));
        }
        if (ESP_OK == status) {
            ESP_LOGI(_log_tag, "%s:%d Calling esp_wifi_start", __func__, __LINE__);
            status = esp_wifi_start();
            ESP_LOGI(_log_tag, "%s:%d esp_wifi_start:%s", __func__, __LINE__,
                                                        esp_err_to_name(status));
        }
        if (ESP_OK == status) {
            ESP_LOGI(_log_tag, "%s:%d INITIALISED", __func__, __LINE__);
            _state = state_e::INITIALIZED;
        }
    } else if (state_e::ERROR == _state) {
        ESP_LOGE(_log_tag, "%s:%d FAILED", __func__, __LINE__);
        _state = state_e::NOT_INITIALIZED;
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