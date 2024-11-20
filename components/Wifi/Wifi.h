#pragma once

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "esp_mac.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include <algorithm>
#include <mutex>

#include <cstring>


namespace WIFI {

class Wifi {
    constexpr static const char* _log_tag{"WiFi"};
public:
    enum class state_e {
        NOT_INITIALIZED,
        INITIALIZED,
        READY_TO_CONNECT,
        CONNECTING,
        WAITING_FOR_IP,
        CONNECTED,
        DISCONNECTED,
        ERROR
    };

    Wifi(void);
    ~Wifi(void)                     = default;
    Wifi(const Wifi&)               = default;
    Wifi(Wifi&&)                    = default;
    Wifi& operator=(const Wifi&)    = default;
    Wifi& operator=(Wifi&&)         = default;

    esp_err_t init(const char* const ssid, const char* const password); // Setup WiFi
    esp_err_t begin(void); // Start WiFi, connect WiFi... etc.

    constexpr static const state_e& get_state(void) {return _state;}
    constexpr static const char* get_mac(void) { return mac_address_cstr; }

private:
    static esp_err_t _init(void);
    static wifi_init_config_t wifi_init_config;
    static wifi_config_t wifi_config;

    void state_machine(void);

    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void sc_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static state_e _state;

    static esp_err_t _get_mac(void);
    static char         mac_address_cstr[13];

    static std::mutex   init_mutex;
    static std::mutex   connect_mutex;
    static std::mutex  state_mutex;
}; // class Wifi
} // namespace WIFI