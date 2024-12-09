#include "esp_log.h"
#include "nvs_flash.h"     
#include "wifi_module.h"
#include "did_module.h" 
#include "psa/crypto_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "utils.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "****** Application Start ******");
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t nvs_err = nvs_flash_init();

    // Connect to the WiFi network
    wifi_init_sta();

    // Generate or retrieve the DID
    did_generate();
    const char *stored_did = did_get();
    // char *test_did = "did:io:0x2e0f48b35635a40f55ae89ef635ca3c4c1ac0f96";
    ESP_LOGI(TAG, "Device DID: %s", stored_did);
    /*
    psa_status_t psa_export_key(psa_key_id_t key,
                            uint8_t *data,
                            size_t data_size,
                            size_t *data_length);

    */
    // let's log the private key
    psa_status_t status;
    uint8_t key_data[512];
    size_t key_data_length;
    status = psa_export_key(1, key_data, sizeof(key_data), &key_data_length);

    if (status == 0) {
        printf("Key exported successfully.\n");
        printf("Key data (hex): ");
        for (size_t i = 0; i < key_data_length; i++) {
            printf("%02X", key_data[i]);
        }
        printf("\n");
    } else {
        printf("Key export failed with status %d\n", status);
    }

    // Extract the address from the DID
    const char *device_address = stored_did + 7; // Skip "did:io:"
    // Check if this device is already registered
    bool registered = is_device_registered(device_address);
    if (!registered) {
        ESP_LOGI(TAG, "Device not registered. Starting registration process...");
        did_register_start();

        // Halt the program until the user reboots the device
        while (1) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } else {
        ESP_LOGI(TAG, "Device already registered.");
        ESP_LOGI(TAG, "I'll destroy the key to generate a new DID....");
        did_destroy_key();
    }
}
// Initialize the NVS (Non-Volatile Storage) module
void nvs_init(void)
{
    ESP_LOGI(TAG, "Initializing NVS...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}