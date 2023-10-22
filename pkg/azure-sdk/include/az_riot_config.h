#ifndef AZ_RIOT_CONFIG_H
#define AZ_RIOT_CONFIG_H

///////////////////////  AZURE /////////////////////
#ifndef AZ_RIOT_HUB_HOST 
#define AZ_RIOT_HUB_HOST "TRHN-HUB.azure-devices.net"
#endif
#ifndef AZ_RIOT_HUB_DEVICEID 
#define AZ_RIOT_HUB_DEVICEID "riot-registration-id01"
#endif
#ifndef AZ_RIOT_DPS_GLOBAL_HOST
#define AZ_RIOT_DPS_GLOBAL_HOST "global.azure-devices-provisioning.net"
#endif
#ifndef AZ_RIOT_DPS_IDSCOPE 
#define AZ_RIOT_DPS_IDSCOPE "0ne00B1BDDA"
#endif
#ifndef AZ_RIOT_DPS_REGISTRATION_ID
#define AZ_RIOT_DPS_REGISTRATION_ID "riot-registration-id01"
#endif
///////////////////////  AZURE /////////////////////

/////////////////////// TLS    /////////////////////

/////////////////////// TLS    /////////////////////

////////////////////// MQTT   /////////////////////
// #define MQTT_VERSION_v311 4 /* MQTT v3.1.1 version is 4 */
#define COMMAND_TIMEOUT_MS 10000
////////////////////// MQTT   /////////////////////

#endif
