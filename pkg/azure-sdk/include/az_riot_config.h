#ifndef AZ_RIOT_CONFIG_H
#define AZ_RIOT_CONFIG_H

///////////////////////  AZURE /////////////////////
#define AZ_RIOT_HUB_HOST "TRHN-HUB.azure-devices.net"
#define AZ_RIOT_HUB_DEVICEID "TRHN-device01"
#define AZ_RIOT_DPS_HOST "global.azure-devices-provisioning.net"
#define AZ_RIOT_DPS_IDSCOPE "0ne00B1BDDA"
#define AZ_RIOT_DPS_REGISTRATION_ID "riot-registrationID"
///////////////////////  AZURE /////////////////////

/////////////////////// TLS    /////////////////////

/////////////////////// TLS    /////////////////////

////////////////////// MQTT   /////////////////////
#define BUF_SIZE 2048
#define MQTT_VERSION_v311 4 /* MQTT v3.1.1 version is 4 */
#define COMMAND_TIMEOUT_MS 4000

#ifndef DEFAULT_MQTT_CLIENT_ID
#define DEFAULT_MQTT_CLIENT_ID "thasan"
#endif

#ifndef DEFAULT_MQTT_USER
#define DEFAULT_MQTT_USER "thasan"
#endif

#ifndef DEFAULT_MQTT_PWD
#define DEFAULT_MQTT_PWD ""
#endif

/**
 * @brief Default MQTT port
 */
#define DEFAULT_MQTT_PORT 8883

/**
 * @brief Keepalive timeout in seconds
 */
#define DEFAULT_KEEPALIVE_SEC 240

#ifndef MAX_LEN_TOPIC
#define MAX_LEN_TOPIC 64
#endif

#ifndef MAX_TOPICS
#define MAX_TOPICS 8
#endif


#define IS_CLEAN_SESSION 0
#define IS_RETAINED_MSG 0
////////////////////// MQTT   /////////////////////

#endif
