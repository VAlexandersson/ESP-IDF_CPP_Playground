#pragma

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"

#include <mutex>

namespace WIFI {

class Wifi {
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

    Wifi(void) {

        std::lock_guard<std::mutex> guard(first_call_mutex);

        if(!first_call) {
            if(ESP_OK != _get_mac()) esp_restart();  
            first_call = true;
        }
    } // constructor Wifi

    esp_err_t init(void); // Set up wifi
    esp_err_t begin(void); // Start WiFi, connect WiFi... etc.

    state_e get_state(void);
    const char* get_mac(void) { return mac_address_cstr; }

private:
    void state_machine(void);

    static char mac_address_cstr[13];
    const esp_err_t _get_mac(void);

    static std::mutex first_call_mutex;
    static bool first_call;
}; // class Wifi


} // namespace WIFI