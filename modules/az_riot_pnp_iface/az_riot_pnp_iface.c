
#include "az_riot_pnp_iface.h"
#include <stdio.h>
#include "mqtts_riot_iface.h"

#include <azure/az_core.h>
#include <azure/az_iot.h>
#include <azure/core/az_span.h>

struct AzRiotContext
{
    MQTTSContext *mqtts_context;
    az_iot_hub_client *iot_hub_client;
    az_iot_provisioning_client *dps_client;
};

static AzRiotContext *azure_context;

void default_test_precondition_failed_callback(void)
{
    // This is the default callback implementation for precondition failed errors
    // You can customize the behavior here, such as logging an error message
    printf("Precondition failed error occurred\n");
}
// static void send_operation_query_message_to_dps( AzRiotContext* context, az_iot_provisioning_client_register_response const* register_response);
int handle_provisioning_message(char *topic, int topic_len, char *payload, int payload_len);
int handle_iot_message(char *topic, int topic_len, char *payload, int payload_len);

static void _on_azure_msg_arival(char *topic, int topic_len, char *payload, int payload_len)
{
    printf("azure msg arival\n Topic : %.*s\nPayload : %.*s\n", topic_len, topic, payload_len, payload);
    int ret = -1;
    if (azure_context->dps_client != NULL)
    {
        ret = handle_provisioning_message(topic, topic_len, payload, payload_len);
    }
    if (ret < 0 && azure_context->iot_hub_client != NULL)
    {
        handle_iot_message(topic, topic_len, payload, payload_len);
    }
}


#ifndef DEFAULT_CERT_DIR_CA
#define DEFAULT_CERT_DIR_CA NULL
#endif
#ifndef DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN
#define DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN NULL
#endif
#ifndef DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY
#define DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY NULL
#endif

int init_mqtts_layer(AzRiotContext *azContext, unsigned char *mqtts_writebuf, int mqtts_writebuf_len, unsigned char *mqtts_readbuf, int mqtts_readbuf_len)
{
    azContext->mqtts_context = mqtts_create_context(NULL);
    int ret = -1;
    if (azContext->mqtts_context == NULL)
    {
        printf("mqtts context failed\n");
        return ret;
    }
    mqtts_set_message_arrival_callback(_on_azure_msg_arival);
    char *ca_cert_path = DEFAULT_CERT_DIR_CA;
    char *cert_chain_path = DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN;
    char *private_key_path = DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY;
    //  mqtts_set_certificate(context, ca_cert_path, cert_chain_path, private_key_path)
    if ((ret = mqtts_set_certificate(azContext->mqtts_context, ca_cert_path, cert_chain_path, private_key_path)) < 0)
    {

        printf("mqtts_set_certificate failed\n");
        return ret;
    }
    ret = mqtts_init(azContext->mqtts_context, mqtts_writebuf, mqtts_writebuf_len, mqtts_readbuf, mqtts_readbuf_len);
    if (ret < 0)
    {
        printf(" mqtts init failed\n");
        return ret;
    }
    return 0;
}

AzRiotContext *create_AzRiotContext(unsigned char *mqtts_writebuf, int mqtts_writebuf_len, unsigned char *mqtts_readbuf, int mqtts_readbuf_len)
{
    AzRiotContext *azContext = (AzRiotContext *)malloc(sizeof(AzRiotContext));
    if (azContext == NULL)
    {
        printf("Failed allocating memory for AzRiotContext\n");
        return NULL;
    }
    az_precondition_failed_set_callback(default_test_precondition_failed_callback);

    int res = init_mqtts_layer(azContext, mqtts_writebuf, mqtts_writebuf_len, mqtts_readbuf, mqtts_readbuf_len);
    if (res < 0)
    {
        printf("Failed init_mqtts_layer\n");
        return NULL;
    }
    azure_context = azContext;
    return azContext;
}

// /////////////////////
// static az_span const provisioning_global_endpoint = AZ_SPAN_LITERAL_FROM_STR("ssl://global.azure-devices-provisioning.net:8883");

// static az_span const mqtt_url_prefix = AZ_SPAN_LITERAL_FROM_STR("ssl://");
// static az_span const mqtt_url_suffix = AZ_SPAN_LITERAL_FROM_STR(":8883");
// //////////////
#define QUERY_TOPIC_BUFFER_LENGTH 256
#define REGISTER_TOPIC_BUFFER_LENGTH 128
#define PROVISIONING_ENDPOINT_BUFFER_LENGTH 256
#define MQTT_PAYLOAD_BUFFER_LENGTH 256
// ///////////////////

int init_iot_hub_client(AzRiotContext *azContext,char* hostName, char* deviceId)
{
    if (azContext->iot_hub_client != NULL)
    {
        printf("iot_hub_client is already initialized\n");
        return 0;
    }
    azContext->iot_hub_client = (az_iot_hub_client *)malloc(sizeof(az_iot_hub_client));
    az_iot_hub_client_options azureOpt = az_iot_hub_client_options_default();
    az_span iotHubHostName = az_span_create((uint8_t*)hostName,strlen(hostName));//az_span_create_from_str(hostAddress);//AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_HUB_HOST);
    az_span iotHubDeviceId = az_span_create((uint8_t*)deviceId,strlen(deviceId));//az_span_create_from_str(deviceId);//AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_HUB_DEVICEID);

    if (az_result_failed(az_iot_hub_client_init(azContext->iot_hub_client, iotHubHostName, iotHubDeviceId, &azureOpt)))
    {
        printf("az_iot_hub_client_init ERROR.\n");
        return -1;
    }
    printf("az_iot_hub_client_init OK!\n");
    return 0;
}

int init_dps_client(AzRiotContext *az_context)
{
    if (az_context->dps_client != NULL)
    {
        printf("az dps client is already initialized\n");
        return 0;
    }
    az_context->dps_client = (az_iot_provisioning_client *)malloc(sizeof(az_iot_provisioning_client));
    // char endpoint_buffer[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
    // int endpoint_buffer_len=0;

    // int res;
    // if ((res = get_info_from_client(az_context, DPS, MQTT_ENDPOINT, endpoint_buffer, PROVISIONING_ENDPOINT_BUFFER_LENGTH,&endpoint_buffer_len)) < 0)
    // {
    //     printf("az dps client failed creating mqtt endpoint\n");
    //     return res;
    // }

    az_iot_provisioning_client_options azureOpt = az_iot_provisioning_client_options_default();

    az_span dpsGloablHostName = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_GLOBAL_HOST); // az_span_create_from_str(endpoint_buffer); //
    az_span dpsRegistrationID = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_REGISTRATION_ID);
    az_span dpsIdScope = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_IDSCOPE);

    az_result azureRes = az_iot_provisioning_client_init(az_context->dps_client, dpsGloablHostName, dpsIdScope, dpsRegistrationID, &azureOpt);
    if (az_result_failed(azureRes))
    {
        printf("az_iot_dps_client_init ERROR!\n");
        return -1;
    }
    printf("az_iot_dps_client_init OK.\n");
    return 0;
}

int connect_dps_client(AzRiotContext *azContext)
{
    if ((azContext == NULL) || (azContext->dps_client == NULL))
    {
        printf("Invalid azure context");
        return -1;
    }
    char client_id_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
    int client_id_buf_len = 0;

    if (az_result_failed(az_iot_provisioning_client_get_client_id(azContext->dps_client, client_id_buf, PROVISIONING_ENDPOINT_BUFFER_LENGTH, NULL)))
    {
        printf("az_iot_provisioning_client_get_client_id failed\n");
        return -1;
    }
    char user_name_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
    int user_name_buf_len = 0;
    if (az_result_failed(az_iot_provisioning_client_get_user_name(azContext->dps_client, user_name_buf, PROVISIONING_ENDPOINT_BUFFER_LENGTH, NULL)))
    {
        printf("az_iot_provisioning_client_get_user_name failed\n");
        return -1;
    }

    int res = mqtts_connect(azContext->mqtts_context, AZ_RIOT_DPS_GLOBAL_HOST, client_id_buf, user_name_buf, NULL);
    if (res != 0)
    {
        printf("mqtt_example: Unable to connect client %d\n", res);
        return res;
    }
    else
    {
        printf("mqtt_example: Connection successfully %d\n", res);
    }
    return res;
}

int connect_iot_hub_client(AzRiotContext *azContext)
{
    if ((azContext == NULL) || (azContext->iot_hub_client == NULL))
    {
        printf("Invalid azure context");
        return -1;
    }
    char client_id_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
    int client_id_buf_len = 0;

    if (az_result_failed(az_iot_hub_client_get_client_id(azContext->iot_hub_client, client_id_buf, PROVISIONING_ENDPOINT_BUFFER_LENGTH, NULL)))
    {
        printf("az_iot_hub_client_get_client_id failed\n");
        return -1;
    }
    char user_name_buf[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
    int user_name_buf_len = 0;
    if (az_result_failed(az_iot_hub_client_get_user_name(azContext->iot_hub_client, user_name_buf, PROVISIONING_ENDPOINT_BUFFER_LENGTH, NULL)))
    {
        printf("az_iot_hub_client_get_user_name failed\n");
        return -1;
    }

    int res = mqtts_connect(azContext->mqtts_context, AZ_RIOT_HUB_HOST, client_id_buf, user_name_buf, NULL);
    if (res != 0)
    {
        printf("mqtt_example: Unable to connect client %d\n", res);
        return res;
    }
    else
    {
        printf("mqtt_example: Connection successfully to IOT HUB %d\n", res);
    }
    return res;
}

int connect_azure_client(AzRiotContext *azContext, azRiotClientType clientType)
{
    switch (clientType)
    {
    case HUB:
        return connect_iot_hub_client(azContext);
    case DPS:
        return connect_dps_client(azContext);
    default:
        break;
    }
    return -1;
}

int register_device_with_provisioning_service(AzRiotContext *azContext)
{
    char register_topic_buffer[REGISTER_TOPIC_BUFFER_LENGTH];
    int rc = az_iot_provisioning_client_register_get_publish_topic(
        azContext->dps_client, register_topic_buffer, sizeof(register_topic_buffer), NULL);
    if (az_result_failed(rc))
    {
        printf("Failed to get the Register topic: az_result return code 0x%08x.\n", rc);
        return -1;
    }

    // Devices registering a ModelId while using Device Provisioning Service must specify
    // their ModelId in an MQTT payload sent during registration.
    char mqtt_payload[MQTT_PAYLOAD_BUFFER_LENGTH];
    size_t mqtt_payload_length = 0;
    snprintf(mqtt_payload, MQTT_PAYLOAD_BUFFER_LENGTH, "{\"registrationId\":\"%s\"}", AZ_RIOT_DPS_REGISTRATION_ID);
    az_iot_provisioning_client_payload_options options = az_iot_provisioning_client_payload_options_default();

    // az_iot_pro
    // _az_PUSH_IGNORE_DEPRECATIONS
    // rc = az_iot_provisioning_client_get_request_payload(
    //     azContext->dps_client,
    //     AZ_SPAN_EMPTY,//custom_registration_payload_property,
    //     &options,
    //     mqtt_payload,
    //     sizeof(mqtt_payload),
    //     &mqtt_payload_length);
    // _az_POP_WARNINGS
    // if (az_result_failed(rc))
    // {
    //     printf(
    //         "Failed to initialize provisioning client: az_result return code 0x%08x.\n", rc);
    //     return -1;
    // }
    printf("*** sending (%d) %.*s\n", mqtt_payload_length, sizeof(mqtt_payload), mqtt_payload);
    // printf("*** sending %.*s\n",mqtt_payload_length,mqtt_payload);
    return mqtts_publish(azContext->mqtts_context, register_topic_buffer, (unsigned char *)mqtt_payload, 1, 0);
}

int subscribe_mqtt_client_to_provisioning_service_topics(AzRiotContext *azContext)
{
    // AZ_IOT_DEFAULT_MQTT_CONNECT_KEEPALIVE_SECONDS
    int ret = mqtts_subscribe(azContext->mqtts_context, AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, 1);
    if (ret < 0)
    {
        printf("dps_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, ret);
    }
    else
    {
        printf("dps_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, 0);
    }
    return ret;
}

// subscribe_mqtt_client_to_iot_hub_topics subscribes to well-known MQTT topics that Azure IoT Hub
// uses to signal incoming commands to the device and notify device of properties.
int subscribe_mqtt_client_to_iot_hub_topics(AzRiotContext *azContext)
{
    // Subscribe to incoming commands
    int ret = mqtts_subscribe(azContext->mqtts_context, AZ_IOT_HUB_CLIENT_COMMANDS_SUBSCRIBE_TOPIC, 1);
    if (ret < 0)
    {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_COMMANDS_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else
    {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_COMMANDS_SUBSCRIBE_TOPIC, 0);
    }
    // Subscribe to property update notifications.  Messages will be sent to this topic when
    // writable properties are updated by the service.
    ret = mqtts_subscribe(azContext->mqtts_context, AZ_IOT_HUB_CLIENT_PROPERTIES_WRITABLE_UPDATES_SUBSCRIBE_TOPIC, 1);
    if (ret < 0)
    {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_WRITABLE_UPDATES_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else
    {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_WRITABLE_UPDATES_SUBSCRIBE_TOPIC, 0);
    }
    // Subscribe to the properties message topic.  When the device invokes a PUBLISH to get
    // all properties (both reported from device and reported - see request_all_properties() below)
    // the property payload will be sent to this topic.
    ret = mqtts_subscribe(azContext->mqtts_context, AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_SUBSCRIBE_TOPIC, 1);
    if (ret < 0)
    {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else
    {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_SUBSCRIBE_TOPIC, 0);
    }
    // Messages received on the C2D topic will be cloud-to-device messages.
    ret = mqtts_subscribe(azContext->mqtts_context, AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, 1);
    if (ret < 0)
    {
        printf("hub_example: Unable to subscribe to %s (%d)\n",
               AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, ret);
        return ret;
    }
    else
    {
        printf("hub_example: Now subscribed to %s, QOS %d\n",
               AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, 0);
    }
    return ret;
}

int subscribe_to_azure_client_topics(AzRiotContext *azContext, azRiotClientType clientType)
{
    switch (clientType)
    {
    case HUB:
        return subscribe_mqtt_client_to_iot_hub_topics(azContext);
    case DPS:
        return subscribe_mqtt_client_to_provisioning_service_topics(azContext);
    default:
        return -1;
    }
}

int send_telemetry_message_to_iot_hub(AzRiotContext *azContext, unsigned char *telemetry_message_payload)
{
    int rc;

    // Get the Telemetry topic to publish the telemetry messages.
    char telemetry_topic_buffer[128];
    rc = az_iot_hub_client_telemetry_get_publish_topic(
        azContext->iot_hub_client, NULL, telemetry_topic_buffer, sizeof(telemetry_topic_buffer), NULL);
    if (az_result_failed(rc))
    {
        printf("Failed to get the Telemetry topic: az_result return code 0x%08x.\n", rc);
        return -1;
    }

    return mqtts_publish(azContext->mqtts_context, telemetry_topic_buffer, telemetry_message_payload, 1, 0);
}

// send_operation_query_message PUBLISHes a message to query
// the status of the registration request.
int send_operation_query_message_to_dps(AzRiotContext *context, char *operation_id)
{
    az_span operation_id_span = az_span_create((uint8_t*)operation_id,strlen(operation_id));
    // AZ_SPAN_FROM_STR((uint8_t*)operation_id);
    // Get the Query Status topic to publish the query status request.
    char query_topic_buffer[QUERY_TOPIC_BUFFER_LENGTH];
    //   int rc = az_iot_provisioning_client_query_status_get_publish_topic(context->dps_client,register_response->operation_id, query_topic_buffer, sizeof(query_topic_buffer), NULL);
    if (az_result_failed(az_iot_provisioning_client_query_status_get_publish_topic(context->dps_client, operation_id_span, query_topic_buffer, sizeof(query_topic_buffer), NULL)))
    {
        printf("send_operation_query_message_to_dps failed\n");
        return -1;
    }
    printf("publishing send_operation_query_message_to_dps to topic %s\n", query_topic_buffer);
    return mqtts_publish(context->mqtts_context, query_topic_buffer, (unsigned char*)"", 0, 0);

    //   // IMPORTANT: Wait the recommended retry-after number of seconds before query.
    //   IOT_SAMPLE_LOG("Querying after %u seconds...", register_response->retry_after_seconds);
    //   iot_sample_sleep_for_seconds(register_response->retry_after_seconds);
}
int handle_provisioning_message(char *topic, int topic_len, char *payload, int payload_len)
{
    az_iot_provisioning_client_register_response register_response;
    // Initialize the incoming topic to a span
    az_span const topic_span = az_span_create((uint8_t *)topic, topic_len);
    az_span const message_span = az_span_create((uint8_t *)payload, payload_len);
    if (az_result_failed(az_iot_provisioning_client_parse_received_topic_and_payload(azure_context->dps_client, topic_span, message_span, &register_response)))
    {
        return -1;
    }
    bool ref_is_operation_complete = az_iot_provisioning_client_operation_complete(register_response.operation_status);

    // If operation is not complete, send query. On return, will loop to receive new operation
    // message.
    if (!ref_is_operation_complete)
    {
        printf("*** Device registration operation is still pending. Please query again after %d seconds with operation ID = %.*s\n",register_response.retry_after_seconds,register_response.operation_id._internal.size, register_response.operation_id._internal.ptr);
        // send_operation_query_message_to_dps(azure_context, &register_response);
    }
    else // Operation is complete.
    {
        if (register_response.operation_status == AZ_IOT_PROVISIONING_STATUS_ASSIGNED) // Successful assignment
        {
            char success_string[512];
            snprintf(success_string, 512,
                     "Device provisioned successfully:\n"
                     "Hub Hostname: %.*s\n"
                     "Device Id: %.*s\n",
                     (int)register_response.registration_state.assigned_hub_hostname._internal.size, register_response.registration_state.assigned_hub_hostname._internal.ptr,
                     (int)register_response.registration_state.device_id._internal.size, register_response.registration_state.device_id._internal.ptr);
            printf("[Registration Success] DPS response = %s\n", success_string);
        }
        else // Unsuccessful assignment (unassigned, failed or disabled states)
        {
            char error_string[256];

            snprintf(error_string, 256,
                     "Device provisioning failed:\n"
                     "Registration state: %d\n"
                     "Last operation status: %d\n"
                     "Operation ID: %.*s\n"
                     "Error code: %u\n"
                     "Error message: %.*s\n"
                     "Error timestamp: %.*s\n"
                     "Error tracking ID: %.*s\n",
                     register_response.operation_status,
                     register_response.status,
                     (int)register_response.operation_id._internal.size, register_response.operation_id._internal.ptr,
                     register_response.registration_state.extended_error_code,
                     (int)register_response.registration_state.error_message._internal.size, register_response.registration_state.error_message._internal.ptr,
                     (int)register_response.registration_state.error_timestamp._internal.size, register_response.registration_state.error_timestamp._internal.ptr,
                     (int)register_response.registration_state.error_tracking_id._internal.size, register_response.registration_state.error_tracking_id._internal.ptr);
            printf("[Registration ERROR] DPS error = %s\n", error_string);
            return -1;
        }
    }
    return 0;
}

int handle_iot_message(char *topic, int topic_len, char *payload, int payload_len)
{
    // Initialize the incoming topic to a span
    az_span incoming_topic = az_span_create((uint8_t *)topic, topic_len);

    // The message could be for three features so parse the topic to see which it is for
    az_iot_hub_client_method_request method_request;
    az_iot_hub_client_c2d_request c2d_request;
    az_iot_hub_client_twin_response twin_response;
    if (az_result_succeeded(az_iot_hub_client_methods_parse_received_topic(azure_context->iot_hub_client, incoming_topic, &method_request)))
    {
        // Handle the method request
        printf("[Unknown] topic arrived [Not Implemented] : %.*s\n", topic_len, topic);
    }
    else if (az_result_succeeded(az_iot_hub_client_c2d_parse_received_topic(azure_context->iot_hub_client, incoming_topic, &c2d_request)))
    {
        // Handle the c2d message
        //  az_span const message_span = az_span_create((uint8_t*)payload, payload_len);
        printf("***** Device : c2d received from Cloud with payload = %.*s\n", payload_len, payload);
    }
    else if (az_result_succeeded(az_iot_hub_client_twin_parse_received_topic(azure_context->iot_hub_client, incoming_topic, &twin_response)))
    {
        // Handle the twin message
        printf("[Unknown] topic arrived [Not Implemented] : %.*s\n", topic_len, topic);
    }
    else
    {
        printf("[Unknown] topic arrived : %.*s\n", topic_len, topic);
        return -1;
    }
    return 0;
}
