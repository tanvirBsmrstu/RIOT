#include <az_riot_config.h>
#include <az_riot.h>
#include <azure/core/az_span.h>
#include <stdio.h>

/////////////////////
static az_span const provisioning_global_endpoint = AZ_SPAN_LITERAL_FROM_STR("ssl://global.azure-devices-provisioning.net:8883");

static az_span const mqtt_url_prefix = AZ_SPAN_LITERAL_FROM_STR("ssl://");
static az_span const mqtt_url_suffix = AZ_SPAN_LITERAL_FROM_STR(":8883");
#define PROVISIONING_ENDPOINT_BUFFER_LENGTH 256
///////////////////

void default_precondition_failed_callback(void)
{
    // This is the default callback implementation for precondition failed errors
    // You can customize the behavior here, such as logging an error message
    printf("Precondition failed error occurred\n");
}

bool Initialize_azClient(az_iot_hub_client *azureClient, azRiotHUB_data *_data)
{
    az_precondition_failed_set_callback(default_precondition_failed_callback);
    if (_data == NULL)
        return false;
    az_result azureRes;
    az_iot_hub_client_options azureOpt = az_iot_hub_client_options_default();
    az_span iotHubHostName = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_HUB_HOST);
    az_span iotHubDeviceId = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_HUB_DEVICEID);
    _data->host = AZ_RIOT_HUB_HOST;
    _data->deviceID = AZ_RIOT_HUB_DEVICEID;
    azureRes = az_iot_hub_client_init(azureClient, iotHubHostName, iotHubDeviceId, &azureOpt);
    if (az_result_succeeded(azureRes))
    {
        printf("az_iot_hub_client_init OK.\n");
    }
    else
    {
        printf("az_iot_hub_client_init ERROR!\n");
        return false;
    }
    azureRes = az_iot_hub_client_get_client_id(azureClient, _data->clientID, sizeof(_data->clientID), &_data->clientID_length);
    if (az_result_succeeded(azureRes))
    {
        printf("az_iot_hub_client_get_client_id OK: %s\n", _data->clientID);
    }
    else
    {
        printf("az_iot_hub_client_get_client_id ERROR!\n");
        return false;
    }
    azureRes = az_iot_hub_client_get_user_name(azureClient, _data->username, sizeof(_data->username), &_data->username_length);
    if (az_result_succeeded(azureRes))
    {
        printf("az_iot_hub_client_get_user_name OK: %s\n", _data->username);
    }
    else
    {
        printf("az_iot_hub_client_get_user_name ERROR!\n");
        return false;
    }
    return true;
}
void create_mqtt_endpoint(azRiotServiceType type, char *out_endpoint, size_t endpoint_size)
{
    if (type == HUB)
    {
        // int32_t const required_size = az_span_size(mqtt_url_prefix)
        //     + az_span_size(AZ_RIOT_HUB_HOST) + az_span_size(mqtt_url_suffix)
        //     + (int32_t)sizeof('\0');

        // if ((size_t)required_size > endpoint_size)
        // {
        //   puts("Failed to create MQTT endpoint: Buffer is too small.");
        //   exit(1);
        // }

        // az_span hub_mqtt_endpoint = az_span_create((uint8_t*)out_endpoint, (int32_t)endpoint_size);
        // az_span remainder = az_span_copy(hub_mqtt_endpoint, mqtt_url_prefix);
        // remainder = az_span_copy(remainder, env_vars->hub_hostname);
        // remainder = az_span_copy(remainder, mqtt_url_suffix);
        // az_span_copy_u8(remainder, '\0');
    }
    else if (type == DPS)
    {

        int32_t const required_size = az_span_size(provisioning_global_endpoint) + (int32_t)sizeof('\0');

        if ((size_t)required_size > endpoint_size)
        {
            puts("Failed to create MQTT endpoint: Buffer is too small.");
        }

        az_span provisioning_mqtt_endpoint = az_span_create((uint8_t *)out_endpoint, (int32_t)endpoint_size);
        az_span remainder = az_span_copy(provisioning_mqtt_endpoint, provisioning_global_endpoint);
        az_span_copy_u8(remainder, '\0');
    }
    else
    {
        puts("Failed to create MQTT endpoint: Sample type undefined.");
    }

    printf("MQTT endpoint created at \"%s\".\n", out_endpoint);
}

bool device_register_device_with_provisioning_service(az_iot_provisioning_client *dpsClient)
{
    int rc;

    // Get the Register topic to publish the register request.
    char register_topic_buffer[60];
    size_t register_topic_buffer_len;
    rc = az_iot_provisioning_client_register_get_publish_topic(
        dpsClient, register_topic_buffer, sizeof(register_topic_buffer), &register_topic_buffer_len);
    if (az_result_failed(rc))
    {
        printf("Failed to get the Register topic: az_result return code 0x%08x.", rc);
        return false; // TODO: exit
    }
    printf("device reg topic %s\n",register_topic_buffer);
    uint8_t payload[256];
    size_t out_payload_len;
    az_iot_provisioning_client_payload_options options = az_iot_provisioning_client_payload_options_default();

    rc = az_iot_provisioning_client_register_get_request_payload(
        dpsClient,
        AZ_SPAN_EMPTY,
        &options,
        payload,
        sizeof(payload),
        &out_payload_len);
        if (az_result_failed(rc))
    {
        printf("Failed to get the Register payload: az_result return code 0x%08x.", rc);
        return false; // TODO: exit
    }
     printf("device reg payload %s\n",payload);
    return true;
}

bool Initialize_azDPS_client(az_iot_provisioning_client *dpsClient, azRiotDPS_data *_data)
{
    az_precondition_failed_set_callback(default_precondition_failed_callback);
    if (_data == NULL)
        return false;
    az_result azureRes;
       create_mqtt_endpoint(DPS,_data->endpoint,100);
    az_iot_provisioning_client_options azureOpt = az_iot_provisioning_client_options_default();
    az_span dpsGloablHostName = az_span_create_from_str(_data->endpoint);//AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_HOST);
    az_span dpsRegistrationID = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_REGISTRATION_ID);
    az_span dpsIdScope = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_DPS_IDSCOPE);
 
    _data->host = AZ_RIOT_DPS_GLOBAL_HOST;
    _data->deviceID = AZ_RIOT_DPS_REGISTRATION_ID;
    _data->idScope = AZ_RIOT_DPS_IDSCOPE;
    // _data->resgistrationID = AZ_RIOT_DPS_REGISTRATION_ID;
    // az_span_create_from_str(_data->endpoint);

    // char mqtt_endpoint_buffer[PROVISIONING_ENDPOINT_BUFFER_LENGTH];
    // create_mqtt_endpoint(DPS,mqtt_endpoint_buffer,sizeof(mqtt_endpoint_buffer));
    //  _data->host=mqtt_endpoint_buffer;
    azureRes = az_iot_provisioning_client_init(dpsClient,dpsGloablHostName, dpsIdScope, dpsRegistrationID, &azureOpt);
    if (az_result_succeeded(azureRes))
    {
        printf("az_iot_dps_client_init OK.\n");
    }
    else
    {
        printf("az_iot_dps_client_init ERROR!\n");
        return false;
    }
    azureRes = az_iot_provisioning_client_get_client_id(dpsClient, _data->clientID, sizeof(_data->clientID), &_data->clientID_length);
    if (az_result_succeeded(azureRes))
    {
        printf("az_iot_dps_client_get_client_id OK: %s\n", _data->clientID);
    }
    else
    {
        printf("az_iot_dps_client_get_client_id ERROR!\n");
        return false;
    }
    azureRes = az_iot_provisioning_client_get_user_name(dpsClient, _data->username, sizeof(_data->username), &_data->username_length);
    if (az_result_succeeded(azureRes))
    {
        printf("az_iot_dps_client_get_user_name OK: %s\n", _data->username);
    }
    else
    {
        printf("az_iot_dps_client_get_user_name ERROR!\n");
        return false;
    }
    
        bool tr = device_register_device_with_provisioning_service(dpsClient);
    return true;
}

// static void subscribe_mqtt_client_to_provisioning_service_topics(void)
// {
//     // Messages received on the Register topic will be registration responses from the server.
//     int rc = MQTTClient_subscribe(mqtt_client, AZ_IOT_PROVISIONING_CLIENT_REGISTER_SUBSCRIBE_TOPIC, 1);
//     if (rc != MQTTCLIENT_SUCCESS)
//     {
//         IOT_SAMPLE_LOG_ERROR(
//             "Failed to subscribe to the Register topic: MQTTClient return code %d.", rc);
//         exit(rc);
//     }
// }


