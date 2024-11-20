#include <stdio.h>
#include "esp_err.h"        
#include "DeviceConnect_Core.h"
#include "deviceregister.h"                        
#include "sprout.h"
#include "deviceregister.h"
#include "did_module.h"
#include "esp_log.h"

static char *myDID = NULL;
static char *myDIDDoc = NULL;

static char *TAG = "did_module";

// Generate a DID and a DID Document
void did_generate(void) {
    ESP_LOGI(TAG, "Generating DID and DID Document...");
    
    // Initialize the DeviceConnect SDK
    default_SetSeed(esp_random());
    iotex_deviceconnect_sdk_core_init(NULL, NULL, NULL);

     // Step 1: Generate the JWK for the device
    unsigned int key_id = 1;

    JWK *signjwk = iotex_jwk_generate(JWKTYPE_EC, JWK_SUPPORT_KEY_ALG_K256,
                                      IOTEX_JWK_LIFETIME_PERSISTENT,
                                      PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_EXPORT,
                                      PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                      &key_id);
    if (NULL == signjwk) {
        printf("Fail to generate a JWK\n");
        return;
    }

    char *sign_jwk_serialize = iotex_jwk_serialize(signjwk, true);
    if (sign_jwk_serialize) {
        printf("JWK[Sign]: \n%s\n", sign_jwk_serialize);
    }

    // Step 2: Generate DID based on JWK
    myDID = iotex_did_generate("io", signjwk);
    if (myDID) {
        printf("DID: \n%s\n", myDID);
    }

    char *myMasterkid = iotex_jwk_generate_kid("io", signjwk);
    if (NULL == myMasterkid) {
        return;
    }

    // Step 3: Generate DIDDoc based on DID
    DIDDoc *diddoc = iotex_diddoc_new();
    if (NULL == diddoc) {
        return;
    }

    did_status_t did_status = iotex_diddoc_property_set(diddoc, IOTEX_DIDDOC_BUILD_PROPERTY_TYPE_CONTEXT, NULL, "https://www.w3.org/ns/did/v1");
    did_status = iotex_diddoc_property_set(diddoc, IOTEX_DIDDOC_BUILD_PROPERTY_TYPE_ID, NULL, myDID);
    if (DID_SUCCESS != did_status) {
        printf("iotex_diddoc_property_set [%d] ret %d\n", IOTEX_DIDDOC_BUILD_PROPERTY_TYPE_ID, did_status);
        return;
    }

    DIDDoc_VerificationMethod *vm_authentication = iotex_diddoc_verification_method_new(diddoc, VM_PURPOSE_AUTHENTICATION, VM_TYPE_DIDURL);
    if (NULL == vm_authentication) {
        printf("Failed to iotex_diddoc_verification_method_new()\n");
    }

    did_status = iotex_diddoc_verification_method_set(vm_authentication, VM_TYPE_DIDURL, myMasterkid);
    if (DID_SUCCESS != did_status) {
        printf("iotex_diddoc_verification_method_set ret %d\n", did_status);
        return;
    }

    VerificationMethod_Map vm_map_1 = iotex_diddoc_verification_method_map_new();
    did_status = iotex_diddoc_verification_method_map_set(vm_map_1, IOTEX_DIDDOC_BUILD_VM_MAP_TYPE_ID, myMasterkid);
    did_status = iotex_diddoc_verification_method_map_set(vm_map_1, IOTEX_DIDDOC_BUILD_VM_MAP_TYPE_TYPE, "JsonWebKey2020");
    did_status = iotex_diddoc_verification_method_map_set(vm_map_1, IOTEX_DIDDOC_BUILD_VM_MAP_TYPE_CON, myDID);
    did_status = iotex_diddoc_verification_method_map_set(vm_map_1, IOTEX_DIDDOC_BUILD_VM_MAP_TYPE_JWK, _did_jwk_json_generate(signjwk));

    DIDDoc_VerificationMethod *vm_vm = iotex_diddoc_verification_method_new(diddoc, VM_PURPOSE_VERIFICATION_METHOD, VM_TYPE_MAP);
    did_status = iotex_diddoc_verification_method_set(vm_vm, VM_TYPE_MAP, vm_map_1);

    myDIDDoc = iotex_diddoc_serialize(diddoc, true);
    if (myDIDDoc) {
        printf("DIDdoc [%d]: \n%s\n", strlen(myDIDDoc), myDIDDoc);
    }
}

void did_register_start(void)
{
    if (myDID == NULL || myDIDDoc == NULL) {
        ESP_LOGE(TAG, "Unable to register: DID and/or DID Document not generated");
        return;
    }

    iotex_pal_sprout_device_register_start(myDID, myDIDDoc);
}

// Get the device DID
char *did_get(void)
{
    if (myDID == NULL) {
        ESP_LOGE(TAG, "DID not generated");
        return NULL;
    }
    return myDID;
}

// Get the device DID Document
char *did_doc_get(void)
{
    if (myDIDDoc == NULL) {
        ESP_LOGE(TAG, "DID Document not generated");
        return NULL;
    }
    return myDIDDoc;
}