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

#include "mqtts_riot_iface.h"

// #define MAIN_QUEUE_SIZE     (8)
// static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

MQTTSContext *context = NULL;

#define BUF_SIZE 1024
static unsigned char writebuf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];

#ifndef DEFAULT_CERT_DIR_CA
#define DEFAULT_CERT_DIR_CA NULL
#endif
#ifndef DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN
#define DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN NULL
#endif
#ifndef DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY
#define DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY NULL
#endif

int init(void)
{
    context = mqtts_create_context(NULL);
    int ret = -1;
    if (context == NULL)
    {
        printf("mqtts context failed\n");
        return ret;
    }
    char *ca_cert_path = DEFAULT_CERT_DIR_CA;
    char *cert_chain_path = DEFAULT_CERT_DIR_DEVICE_CERT_CHAIN;
    char *private_key_path = DEFAULT_CERT_DIR_DEVICE_PRIVATE_KEY;
    //  mqtts_set_certificate(context, ca_cert_path, cert_chain_path, private_key_path)
    if ((ret = mqtts_set_certificate(context, ca_cert_path, cert_chain_path, private_key_path)) < 0)
    {

        printf("mqtts_set_certificate failed\n");
        return ret;
    }
    ret = mqtts_init(context, writebuf, BUF_SIZE, readbuf, BUF_SIZE);
    if (ret < 0)
    {
        printf(" mqtts init failed\n");
        return ret;
    }
    return 0;
}

static int _cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mqtts_disconnect(context);
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

    char *remote_ip = argv[1];
    char *remoteAddress = "global.azure-devices-provisioning.net";
    char *clientID = "riot-registration-id01";
    char *username = "0ne00B1BDDA/registrations/riot-registration-id01/api-version=2019-03-31";

    printf("connecting ....\n");
    int ret = mqtts_connect(context, remoteAddress, clientID, username, NULL);
    if (ret != 0)
    {
        printf("mqtt_example: Unable to connect client %d\n", ret);
        _cmd_discon(0, NULL);
        return ret;
    }
    else
    {
        printf("mqtt_example: Connection successfully %d\n", ret);
    }

    return ret;
}

static int _cmd_pub(int argc, char **argv)
{
    // enum QoS qos = QOS0;
    char* topic = "$dps/registrations/PUT/iotdps-register/?$rid={request_id}";
    char* payload = "{\"registrationId\":\"riot-registration-id01\"}";
    
    int rc;
    if ((rc = mqtts_publish(context,topic,payload,0,0)) < 0) {
        printf("mqtt_example: Unable to publish (%d)\n", rc);
    }
    else {
        printf("mqtt_example: Message (%s) has been published to topic %s"
               "with QOS %d\n",
               payload, topic, 0);
    }
    return rc;
}

static int _cmd_sub(int argc, char **argv)
{
    // enum QoS qos = QOS0;
    char* topic = "$dps/registrations/res/#";
    int ret = mqtts_subscribe(context,topic,0);
    if (ret < 0) {
        printf("mqtts_example: Unable to subscribe to %s (%d)\n",
               topic, ret);
        // _cmd_discon(0, NULL);
    }
    else {
        printf("mqtts_example: Now subscribed to %s, QOS %d\n",
               topic, 0);
    }
    return 0; // ret;
}

static int _cmd_unsub(int argc, char **argv)
{

    char* topic = "$dps/registrations/res/#";

    int ret = mqtts_unsubscribe(context,topic);

    if (ret < 0) {
        printf("mqtts_example: Unable to unsubscribe from topic: %s\n", topic);
        // _cmd_discon(0, NULL);
    }
    else {
        printf("mqtts_example: Unsubscribed from topic:%s\n", topic);
    }
    return ret; 
}

static const shell_command_t shell_commands[] =
    {
        {"con", "connect to MQTT broker", _cmd_con},
        {"discon", "disconnect from the current broker", _cmd_discon},
        {"pub", "publish something", _cmd_pub},
        {"sub", "subscribe topic", _cmd_sub},
        {"unsub", "unsubscribe from topic", _cmd_unsub},
        {NULL, NULL, NULL}};

int main(void)
{
    // if (IS_USED(MODULE_GNRC_ICMPV6_ECHO)) {
    //     msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    // }
#ifdef MODULE_LWIP
    /* let LWIP initialize */
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
#endif

    init();

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
