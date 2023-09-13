#include <az_riot_config.h>
#include <az_riot.h>
#include <azure/core/az_span.h>
#include <stdio.h>




void default_precondition_failed_callback(void)
{
    // This is the default callback implementation for precondition failed errors
    // You can customize the behavior here, such as logging an error message
    printf("Precondition failed error occurred\n");
}

bool Initialize_azClient(az_iot_hub_client *azureClient, azRiotData *_data)
{
    az_precondition_failed_set_callback(default_precondition_failed_callback);
    if(_data==NULL) return false;
    az_result azureRes;
    az_iot_hub_client_options azureOpt = az_iot_hub_client_options_default();
    az_span iotHubHostName = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_HUB_HOST);
    az_span iotHubDeviceId = AZ_SPAN_LITERAL_FROM_STR(AZ_RIOT_HUB_DEVICEID);
    _data->host=AZ_RIOT_HUB_HOST;
    _data->deviceID=AZ_RIOT_HUB_DEVICEID;
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

