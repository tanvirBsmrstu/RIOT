
#ifndef AZ_RIOT_H
#define AZ_RIOT_H
#include <azure/az_core.h>
#include <azure/az_iot.h>

typedef struct azRiotData{
    char* host;
    char* deviceID;
    char username[128];
    size_t username_length;
    char clientID[16];
    size_t clientID_length;
}azRiotData;
#endif
void default_precondition_failed_callback(void);
bool Initialize_azClient(az_iot_hub_client*, azRiotData*);

