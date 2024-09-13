#pragma once

#include <cstring>

#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"

namespace NVS {

class Nvs {
    const char* const _log_tag{nullptr};
    nvs_handle_t _handle{};
    const char* const partition_name{nullptr};

public:
    constexpr Nvs(const char* const partition_name = "nvs") : 
        _log_tag{partition_name}, partition_name{partition_name} {}

    [[nodiscard]] esp_err_t init(void) { return _open_partition(partition_name, handle);}

    // Type
    template<typename T>
    [[nodiscard]] esp_err_t get(const char* const key, T& output);
    template<typename T>
    [[nodiscard]] esp_err_t set(const char* const key, const T& value);
    template<typename T>
    [[nodiscard]] esp_err_t verify(const char* const key, const T& value);

    // Buffer
    template<typename T>
    [[nodiscard]] esp_err_t get_buffer(const char* const key, T* output, const size_t len);
    template<typename T>
    [[nodiscard]] esp_err_t set_buffer(const char* const key, const T input, const size_t len);
    template<typename T>
    [[nodiscard]] esp_err_t verify_buffer(const char* const key, const T input, const size_t len);

private:
    [[nodiscard]] static esp_err_t _open(const char* const partition_name, nvs_handle_t& handle)
        { return nvs_open(partition_name, NVS_READWRITE, &handle); }

    template<typename T>
    [[nodiscard]] static esp_err_t _get(nvs_handle_t handle, const char* const key, T& output) {
        size_t n_bytes{sizeof(T)};

        if (nullptr == key || 0 == strlen(key))
            return ESP_ERR_INVALID_ARG;
        else
            return _get_buffer(handle, key, &output, n_bytes);

    }

    template<typename T>
    [[nodiscard]] static esp_err_t _set(nvs_handle_t handle, const char* const key, T& output) {
        size_t n_bytes{sizeof(T)};

        if (nullptr == key || 0 == strlen(key))
            return ESP_ERR_INVALID_ARG;
        else {
            esp_err_t status{_set_buffer(handle, key, &output, n_bytes)};

            if (ESP_OK == status) status |= nvs_commit(handle);
            if (ESP_OK == status) status |= _verify(handle, key, &output, n_bytes);

            return status;
        }

    }

    template<typename T>
    [[nodiscard]]static esp_err_t _verify(nvs_handle_t handle, const char* const key, T& input) {

        T val_in_nvs{};
        esp_err_t status = _get(handle, key, val_in_nvs);

        if (ESP_OK == status) {
            if(input == val_in_nvs)
                return ESP_OK;
            else
                return ESP_FAIL;    
        }
        else
            return status;
    }

    template<typename T>
    [[nodiscard]] static esp_err_t _get_buf(nvs_handle_t handle, const char* const key, T* output, const size_t len) {
        size_t n_bytes{sizeof(T) * len};

        if (nullptr == key || 0 == strlen(key) || nullptr == output || 0 == len)
            return ESP_ERR_INVALID_ARG;
        else
            return nvs_get_blob(handle, key, static_cast<void*>(output), &n_bytes);
    }

};

} // namespace NVS
