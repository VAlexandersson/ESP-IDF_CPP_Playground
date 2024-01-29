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
    constexpr static const char* ssid{"WifiSSID"};
    constexpr static const char* password{"WifiPassword"};
public:
    enum class state_e {
        NOT_INITIALIZED,
        INITIALIZED,
        WAITING_FOR_CREDENTIALS,
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

    esp_err_t init(void); // Set up wifi
    esp_err_t begin(void); // Start WiFi, connect WiFi... etc.

    state_e get_state(void);

    constexpr static const char* get_mac(void) { return mac_address_cstr; }

private:
// FUNCTIONS

    static esp_err_t _init(void);
    static wifi_init_config_t wifi_init_config;
    static wifi_config_t wifi_config;

    void state_machine(void);

    static esp_err_t _get_mac(void);

// VARIABLES
    static state_e      _state;
    static char         mac_address_cstr[13];
    static std::mutex   init_mutex;

}; // class Wifi


} // namespace WIFI