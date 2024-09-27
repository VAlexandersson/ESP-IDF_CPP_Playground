#include "Tft.h"
#include "esp_log.h"
#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#define LOG_TAG "TFT"

// Hardcoded SPI and GPIO configuration (for this example)
#define TFT_SPI_HOST HSPI_HOST
#define TFT_SCLK_PIN 18
#define TFT_MOSI_PIN 23
#define TFT_MISO_PIN 19
#define TFT_CS_PIN GPIO_NUM_5
#define TFT_DC_PIN GPIO_NUM_2
#define TFT_RST_PIN GPIO_NUM_4

namespace Display {

Tft::Tft() :
    _dc(TFT_DC_PIN, false),
    _rst(TFT_RST_PIN, false) { }


esp_err_t Tft::init(void) {
    esp_err_t ret;

    spi_bus_config_t buscfg{
        .mosi_io_num = TFT_MOSI_PIN,
        .miso_io_num = TFT_MISO_PIN,
        .sclk_io_num = TFT_SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0 // Default
    };

    spi_device_interface_config_t devcfg{
        .mode = 0, // SPI mode 0
        .clock_speed_hz = 10 * 1000 * 1000, // 10 MHz (adjust as needed)
        .spics_io_num = TFT_CS_PIN,
        .queue_size = 7, // We want to be able to queue 7 transactions at a time
        .pre_cb = nullptr, // Specify pre-transfer callback to handle D/C line
    };

    // Initialize the SPI bus
    ret = spi_bus_initialize(TFT_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(TFT_SPI_HOST, &devcfg, &_spi);
    ESP_ERROR_CHECK(ret);

    // Initialize the non-SPI GPIOs
    _dc.init();
    _rst.init();


    // Reset the display
    _rst.set(false);
    vTaskDelay(pdMS_TO_TICKS(100));
    _rst.set(true);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Send initialization commands (replace with your ILI9341 commands)
    sendCommand(0x01); // Software reset
    vTaskDelay(pdMS_TO_TICKS(150)); 
    sendCommand(0x28); // Display off
    // ... (add other initialization commands from the datasheet) ...

    sendCommand(0x29); // Display on
    // ... (add other initialization commands from the datasheet) ...

    return ESP_OK;
}

void Tft::clear(const uint16_t color) {
    // ... (Implementation to fill the entire screen with the specified color) ...
    // This will involve setting the drawing window and sending pixel data
}

void Tft::sendCommand(const uint8_t command) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // Zero out the transaction
    t.length = 8; // Command is 8 bits
    t.tx_buffer = &command; // The data is the cmd itself
    t.user = (void*)0; // D/C needs to be set to 0
    ret = spi_device_polling_transmit(_spi, &t); // Transmit!
    assert(ret == ESP_OK); // Should have had no issues.
}

void Tft::sendData(const uint8_t data) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // Zero out the transaction
    t.length = 8; // Len is in bytes, transaction length is in bits.
    t.tx_buffer = &data; // Data
    t.user = (void*)1; // D/C needs to be set to 1
    ret = spi_device_polling_transmit(_spi, &t); // Transmit!
    assert(ret == ESP_OK); // Should have had no issues.
}

} // namespace Display