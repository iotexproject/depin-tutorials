#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"

static const char *TAG = "UTILS";

#define ETH_RPC_URL "https://babel-api.testnet.iotex.io"

// Function to encode the address parameter
void encode_address(const char *address, char *encoded_data, size_t size) {
    snprintf(encoded_data, size, "f6a3d24e000000000000000000000000%s", address + 2); // Skip '0x'
}

// Function to call `exists(address)` using HTTPS
bool is_device_registered(const char *device_address) {
    // HTTP client configuration
    esp_http_client_config_t config = {
        .url = ETH_RPC_URL, // Replace with your actual URL
        .method = HTTP_METHOD_POST,
        .buffer_size = 512,
        .buffer_size_tx = 512,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return false;
    }

    // Create the JSON-RPC payload
    char payload[512] = {0};
    snprintf(payload, sizeof(payload),
             "{\"jsonrpc\":\"2.0\",\"method\":\"eth_call\","
             "\"params\":[{\"to\":\"0x0A7e595C7889dF3652A19aF52C18377bF17e027D\","
             "\"data\":\"f6a3d24e000000000000000000000000%s\"},\"latest\"],\"id\":1}",
             &device_address[2]); // Remove "0x" prefix from device_address

    ESP_LOGI(TAG, "Payload: %s", payload);
    ESP_LOGI(TAG, "Endpoint: %s", ETH_RPC_URL);
    // Open the HTTP connection
    esp_err_t err = esp_http_client_open(client, strlen(payload));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return false;
    }
    ESP_LOGI(TAG, "HTTP Connection OK");
    // Write the request payload
    int wlen = esp_http_client_write(client, payload, strlen(payload));
    if (wlen < 0) {
        ESP_LOGE(TAG, "Failed to write request body");
        esp_http_client_cleanup(client);
        return false;
    }

    // Fetch the headers
    int content_length = esp_http_client_fetch_headers(client);
    if (content_length < 0) {
        ESP_LOGE(TAG, "HTTP client fetch headers failed");
        esp_http_client_cleanup(client);
        return false;
    }

    // Read the HTTP response
    char output_buffer[512] = {0};
    int data_read = esp_http_client_read_response(client, output_buffer, sizeof(output_buffer) - 1);
    if (data_read >= 0) {
        output_buffer[data_read] = '\0'; // Null-terminate the response
        ESP_LOGI(TAG, "Response: %s", output_buffer);

        // Parse the response for the "result" field
        const char *result_key = "\"result\":\"";
        char *result_start = strstr(output_buffer, result_key);
        if (result_start) {
            result_start += strlen(result_key); // Move to the start of the value
            char *result_end = strchr(result_start, '"');
            if (result_end) {
                *result_end = '\0'; // Null-terminate the result
                ESP_LOGI(TAG, "Parsed result: %s", result_start);
                esp_http_client_cleanup(client);

                return strcmp(result_start, "0x0000000000000000000000000000000000000000000000000000000000000001") == 0; // Return true if the result is "0x1"
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to read response");
    }

    // Clean up and return false if anything failed
    esp_http_client_cleanup(client);
    return false;
}