#ifndef AZ_RIOT_H
#define AZ_RIOT_H
#include <azure/az_core.h>
#include <azure/az_iot.h>

typedef struct azRiotHUB_data{
    char* host;
    char* deviceID;
    char username[128];
    size_t username_length;
    char clientID[16];
    size_t clientID_length;
}azRiotHUB_data;

typedef struct azRiotDPS_data{
    char* host;
    char* deviceID;
    char* idScope;
    char* resgistrationID;
    char username[128];
    size_t username_length;
    char clientID[60];
    size_t clientID_length;
}azRiotDPS_data;

typedef enum {
    DPS = 0,
    HUB
}azRiotServiceType;

#endif
void default_precondition_failed_callback(void);
bool Initialize_azClient(az_iot_hub_client*, azRiotHUB_data*);
bool Initialize_azDPS_client(az_iot_provisioning_client *dpsClient, azRiotDPS_data *_data);
void create_mqtt_endpoint(azRiotServiceType type,char* out_endpoint,size_t endpoint_size);
// bool register_device_with_provisioning_service(az_iot_provisioning_client *dpsClient, char *register_topic_buffer,size_t* out_register_topic_buffer_len, uint8_t *payload,size_t* out_payload_len)
// bool device_register_device_with_provisioning_service(az_iot_provisioning_client *dpsClient)
