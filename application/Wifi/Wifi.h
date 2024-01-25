#pragma

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"

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
        if(ESP_OK != _get_mac()) abort(); // improve! 
    }

    esp_err_t init(void); // Set up wifi
    esp_err_t begin(void); // Start WiFi, connect WiFi... etc.

    state_e get_state(void);
    const char* get_mac(void) { return mac_address_ctr; }

private:
    void state_machine(void);

    static char mac_address_ctr[13];
    const esp_err_t _get_mac(void);
}; // class Wifi


} // namespace WIFI