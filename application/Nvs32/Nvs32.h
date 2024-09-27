#pragma once

#include <cstring>

#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"

namespace NVS {

class Nvs {
    const char* const _log_tag{nullptr};
    nvs_handle_t handle{};
    const char* const partition_name{nullptr};

public:
    constexpr Nvs(const char* const partition_name = "nvs") : 
        _log_tag{partition_name}, partition_name{partition_name} {}

    [[nodiscard]] esp_err_t init(void) { return _open(partition_name, handle);}

    // Type
    template<typename T>
    [[nodiscard]] esp_err_t get(const char* const key, T& output) { 
      size_t len = 1; 
      return _get_buf(handle, key, &output, len);
    }

    template<typename T>
    [[nodiscard]] esp_err_t set(const char* const key, const T& input)
      { return _set_buf(handle, key, &input, 1); }
    template<typename T>
    [[nodiscard]] esp_err_t verify(const char* const key, const T& input)
      { return _verify(handle, key, &input, 1); }

    // Buffer
    template<typename T>
    [[nodiscard]] esp_err_t get_buffer(const char* const key, T* output, size_t& len)
      { return _get_buf(handle, key, output, len); }

    template<typename T>
    [[nodiscard]] esp_err_t set_buffer(const char* const key, const T input, const size_t len)
      { return _set_buf(handle, key, input, len); }

    template<typename T>
    [[nodiscard]] esp_err_t verify_buffer(const char* const key, const T input, const size_t len)
      { return _verify_buf(handle, key, input, len); }

private:
    [[nodiscard]] static esp_err_t _open(const char* const partition_name, nvs_handle_t& handle)
        { return nvs_open(partition_name, NVS_READWRITE, &handle); }

    template<typename T>
    [[nodiscard]] static esp_err_t _get_buf(nvs_handle_t handle, const char* const key, T* output, size_t& len) {
      size_t n_bytes{sizeof(T) * len};

      if (nullptr == key || 0 == strlen(key) || nullptr == output || 0 == len)
        return ESP_ERR_INVALID_ARG;
      else {

        const esp_err_t status = nvs_get_blob(handle, key, output, &n_bytes); 

        len = n_bytes / sizeof(T); // Update len to reflect actual number of elements read

        return status;
      }
    }

    template<typename T>
    [[nodiscard]] static esp_err_t _set_buf(nvs_handle_t handle, const char* const key, const T* input, const size_t& len) {
      esp_err_t status{ESP_OK};

      if (nullptr == key || 0 == strlen(key) || nullptr == input || 0 == len)
        status =  ESP_ERR_INVALID_ARG;
      else {
        status = nvs_set_blob(handle, key, input, (sizeof(T) * len)); // API writes to RAM, not flash

        if (ESP_OK == status)
          status = nvs_commit(handle); // Force flush to write to flash so we can verify buffer

        if (ESP_OK == status)
          status = _verify_buf(handle, key, input, len);
      }

      return status;
    }

    template <typename T>
    [[nodiscard]] static esp_err_t _verify_buf(nvs_handle_t handle, const char* const key, const T* input, const size_t len) {

      esp_err_t status{ESP_OK};

      T* buf_in_nvs = new T[len]{};
      size_t n_items_in_nvs = len;

      if (buf_in_nvs) {
        status = _get_buf(handle, key, buf_in_nvs, n_items_in_nvs);

        if (ESP_OK == status) {
          if (len == n_items_in_nvs) {
            if (0 != memcmp(input, buf_in_nvs, len * sizeof(T))) // memcmp returns amount of bytes that differ
              status = ESP_FAIL;
          }
          else
            status = ESP_ERR_NVS_INVALID_LENGTH;
        }
        delete[] buf_in_nvs;
      }
      else 
        status = ESP_ERR_NO_MEM;
      return status;
    }
};

} // namespace NVS