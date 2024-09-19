#include "Sntp32.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"



namespace SNTP {

std::chrono::_V2::system_clock::time_point Sntp::last_update{};
Sntp::time_source_e Sntp::source{Sntp::time_source_e::TIME_SRC_UNKNOWN};
bool Sntp::_running{false};

void Sntp::callback_on_ntp_update(timeval *tv) {
    ESP_LOGI(_log_tag, "Time is %s", ascii_time_now());
}


esp_err_t Sntp::init(void) {
    if(!_running) {
        setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
        tzset();

        sntp_setoperatingmode(SNTP_OPMODE_POLL);

        sntp_setservername(0, "time.google.com");
        sntp_setservername(1, "pool.ntp.org");

        sntp_set_time_sync_notification_cb(&callback_on_ntp_update);
        sntp_set_sync_interval(60*1000); // make variable

        sntp_init();

        source = TIME_SRC_NTP;

        _running = true;
    }

    if(_running)
        return ESP_OK;

    return ESP_FAIL;
}

[[nodiscard]] const char* Sntp::ascii_time_now(void) {
    const std::time_t time_now {std::chrono::system_clock::to_time_t(time_point_now())};

    return std::asctime(std::localtime(&time_now));
}

} // namespace SNTP