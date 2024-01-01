/*
 * Copyright (C) 2019 Javier FILEIV <javier.fileiv@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file        main.c
 * @brief       Example using MQTT Paho package from RIOT
 *
 * @author      Javier FILEIV <javier.fileiv@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "timex.h"
#include "ztimer.h"
#include "shell.h"
#include "thread.h"
#include "mutex.h"

#include "az_riot_pnp_iface.h"

// #define MAIN_QUEUE_SIZE     (8)
// static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define BUF_SIZE 1024
static unsigned char writebuf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];

// #ifndef DEFAULT_CERT_DIR_CA
// #define DEFAULT_CERT_DIR_CA NULL
// #endif
// #ifndef DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN
// #define DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN NULL
// #endif
// #ifndef DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY
// #define DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY NULL
// #endif

AzRiotContext *context = NULL;
int azInit(void)
{
    context = create_AzRiotContext(writebuf, BUF_SIZE, readbuf, BUF_SIZE);
    if (context == NULL)
    {
        printf("create_AzRiotContext\n");
        return -1;
    }
    int res = init_dps_client(context);
    if (res < 0)
    {
        printf("init_iot_hub_client failed with %d\n", res);
        return res;
    }
    return 0;
}

int init(void)
{
    // context = mqtts_create_context(NULL);
    // int ret = -1;
    // if (context == NULL)
    // {
    //     printf("mqtts context failed\n");
    //     return ret;
    // }
    // char *ca_cert_path = DEFAULT_CERT_DIR_CA;
    // char *cert_chain_path = DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN;
    // char *private_key_path = DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY;
    // //  mqtts_set_certificate(context, ca_cert_path, cert_chain_path, private_key_path)
    // if ((ret = mqtts_set_certificate(context, ca_cert_path, cert_chain_path, private_key_path)) < 0)
    // {

    //     printf("mqtts_set_certificate failed\n");
    //     return ret;
    // }
    // ret = mqtts_init(context, writebuf, BUF_SIZE, readbuf, BUF_SIZE);
    // if (ret < 0)
    // {
    //     printf(" mqtts init failed\n");
    //     return ret;
    // }
    return 0;
}

static int _connect_iot_hub(int argc, char **argv)
{
    if(argc<3){

        printf("usage: %s <hub addr> [deviceID]\n", argv[0]);
        return 1;
    }

    int res = init_iot_hub_client(context,argv[1],argv[2]);
    if (res < 0)
    {
        printf("init_iot_hub_client failed with %d\n", res);
        return res;
    }
    printf("riot hub init successfull");
     res = connect_azure_client(context, HUB);
    if (res != 0)
    {
        printf("mqtt_example: Unable to connect client return code =%d\n", res);
        // _cmd_discon(0, NULL);
        return res;
    }
    else
    {
        printf("IoT Hub : Connection successfull\n");
    }
    return res;
}

static int _cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // mqtts_disconnect(context->mqtts_context);
    // if (res < 0) {
    //     printf("mqtt_example: Unable to disconnect\n");
    // }
    // else {
    //     printf("mqtt_example: Disconnect successful\n");
    // }
    return 0;
}

static int _cmd_con(int argc, char **argv)
{
    // if (argc < 2) {
    //     printf(
    //         "usage: %s <brokerip addr> [port] [clientID] [user] [password] "
    //         "[keepalivetime]\n",
    //         argv[0]);
    //     return 1;
    // }

    // char *remote_ip = argv[1];
    // char *remoteAddress = "global.azure-devices-provisioning.net";
    // char *clientID = "riot-registration-id01";
    // char *username = "0ne00B1BDDA/registrations/riot-registration-id01/api-version=2019-03-31";

    printf("connecting ....\n");
    int ret = connect_azure_client(context, DPS);
    if (ret != 0)
    {
        printf("Unable to connect client %d\n", ret);
        // _cmd_discon(0, NULL);
        return ret;
    }
    else
    {
        printf("DPS : Connection successfull\n");
    }

    return ret;
}

static int _cmd_register_device(int argc, char **argv)
{

    char *telemetry_message_payload = "{\"hub_test_demo\":\"riot-os-hub-test-show-demo\"}";
    // // enum QoS qos = QOS0;
    char *topic = "send_telemetry_message_to_iot_hub";
    // // char* payload = "{\"registrationId\":\"riot-registration-id01\"}";

    // int rc;
    // if ((rc = send_telemetry_message_to_iot_hub(context,telemetry_message_payload)) < 0) {
    //     printf("top_example: Unable to publish (%d)\n", rc);
    // }
    // else {
    //     printf("mqtt_example: Message (%s) has been published to topic %s"
    //            "with QOS %d\n",
    //            telemetry_message_payload, topic, 0);
    // }
    int rc;
    if ((rc = register_device_with_provisioning_service(context)) < 0)
    {
        printf("azure_example: Unable to publish (%d)\n", rc);
    }
    else
    {
        printf("azure_example: Message has been published with payload => %s \n",telemetry_message_payload);
    }
    return rc;
}
static int _cmd_send_telemetry(int argc, char **argv)
{

    char *telemetry_message_payload = "{\"riot_message\":\" This message is sent from riot device\"}";
    // // enum QoS qos = QOS0;
    char *topic = "send_telemetry_message_to_iot_hub";
    // char* payload = "{\"registrationId\":\"riot-registration-id01\"}";

    int rc;
    if ((rc = send_telemetry_message_to_iot_hub(context,(unsigned char*)telemetry_message_payload)) < 0) {
        printf("azure_example: Unable to publish (%d)\n", rc);
    }
    else {
        printf("azure_example: Message has been sent with payload => %s\n",telemetry_message_payload);
    }
    return rc;
}

static int _cmd_device_reg_query(int argc, char **argv)
{
    if(argc < 2) {
        printf("usage: %s [operationID]\n", argv[1]);
        return 1;
    }

    if (send_operation_query_message_to_dps(context, argv[1]) < 0)
    {
        printf("send_operation_query_message_to_dps failed\n");
    }
    return 0;
}

static int _cmd_sub_dps(int argc, char **argv)
{
    // enum QoS qos = QOS0;
  

    int ret = subscribe_to_azure_client_topics(context, DPS);
    if (ret < 0)
    {
        printf("azure_example: Unable to subscribe to DPS topic, return code = %d\n", ret);
        // _cmd_discon(0, NULL);
    }
    else
    {
        printf("azure_example: Now subscribed to DPS topic\n");
    }
    return ret;
}
static int _cmd_sub_hub(int argc, char **argv)
{
    char *topic = "HUB";
    int ret = subscribe_to_azure_client_topics(context, HUB);
    if (ret < 0)
    {
        printf("azure_example: Unable to subscribe to HUB topic, return code = %d\n", ret);
        // _cmd_discon(0, NULL);
    }
    else
    {
         printf("azure_example: Now subscribed to HUB topics\n");
    }
    return ret;
}

static int _cmd_unsub(int argc, char **argv)
{

    // char* topic = "$dps/registrations/res/#";

    // int ret = mqtts_unsubscribe(context,topic);

    // if (ret < 0) {
    //     printf("mqtts_example: Unable to unsubscribe from topic: %s\n", topic);
    //     // _cmd_discon(0, NULL);
    // }
    // else {
    //     printf("mqtts_example: Unsubscribed from topic:%s\n", topic);
    // }
    return 0;
}

static const shell_command_t shell_commands[] =
    {
        {"con_dps", "connect to DPS", _cmd_con},
        {"discon", "disconnect from the current broker", _cmd_discon},
        {"reg_device", "device provisioning with DPS", _cmd_register_device},
        {"sub_hub", "subscribe to hub topics", _cmd_sub_hub},
        {"sub_dps", "subscribe to dps topic", _cmd_sub_dps},
        {"unsub", "unsubscribe from topic", _cmd_unsub},
        {"query", "device registration query", _cmd_device_reg_query},
        {"con_hub", "connect to iot hub", _connect_iot_hub},
        {"send_telemetry", "send telemetry to iot hub", _cmd_send_telemetry},
        {NULL, NULL, NULL}};
#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
int main(void)
{
    if (IS_USED(MODULE_GNRC_ICMPV6_ECHO))
    {
        msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    }
#ifdef MODULE_LWIP
    /* let LWIP initialize */
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
#endif
    int ret = 0;
    if ((ret = azInit()) < 0)
    {
        printf("az init failed\n");
    }
    else
    {

        printf("azure sdk initilization successfull\n");
    }

    // NetworkInit(&network);

    // MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE,
    //                readbuf,
    //    BUF_SIZE);
    printf("Running mqtt paho example. Type help for commands info\n");

    // MQTTStartTask(&client);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
