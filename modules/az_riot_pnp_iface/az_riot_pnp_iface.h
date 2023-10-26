#ifndef AZ_RIOT_PNP_IFACE_H
#define AZ_RIOT_PNP_IFACE_H

#include <az_riot_config.h>

struct AzRiotContext;
typedef struct AzRiotContext AzRiotContext;

typedef enum {
    DPS = 0,
    HUB
}azRiotClientType;

typedef enum {
    CLIENT_ID = 0,
    USER_NAME,
    MQTT_ENDPOINT
}azRiotQueryWrapper;

AzRiotContext *create_AzRiotContext(unsigned char* mqtts_writebuf,int mqtts_writebuf_len,unsigned char* mqtts_readbuf,int mqtts_readbuf_len);

int init_dps_client(AzRiotContext* azContext);
int init_iot_hub_client(AzRiotContext *azContext,  char* hostAddress,  char* deviceId);

// // int init_mqtts_layer(AzRiotContext* azContext);

// static az_span custom_registration_payload_property
//     = AZ_SPAN_LITERAL_FROM_STR("{\"modelId\":\"dtmi:com:example:Thermostat;1\"}");

int send_telemetry_message_to_iot_hub(AzRiotContext *azContext, unsigned char *telemetry_message_payload);
int register_device_with_provisioning_service(AzRiotContext *azContext);

int connect_azure_client(AzRiotContext* azContext,azRiotClientType clientType);
// //int publish_azure_client(AzRiotContext* azContext,azRiotClientType clientType);
int subscribe_to_azure_client_topics(AzRiotContext* azContext,azRiotClientType clientType);
// int disconnect_azure_client(AzRiotContext* azContext,azRiotClientType clientType);

// void default_test_precondition_failed_callback(void);
int send_operation_query_message_to_dps(AzRiotContext *context, char *operation_id);


#endif
