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
#include <az_riot_config.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "timex.h"
#include "ztimer.h"
#include "shell.h"
#include "thread.h"
#include "mutex.h"
#include "paho_mqtt.h"
#include "MQTTClient.h"

#include "net/ipv6/addr.h"

// #include "az_riot.h"

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static char tcp_stack[THREAD_STACKSIZE_DEFAULT];
static char tls_stack[THREAD_STACKSIZE_DEFAULT];

#define SERVER_IP "fec0:affe::100"
#define CLIENT_IP "fec0:affe::99"



static MQTTClient client;
static Network network;

#include <az_riot.h>
#include <azure/az_core.h>
#include <azure/az_iot.h>


azRiotData azctx;
az_iot_hub_client my_client; 

void InitializeAZ(){
    bool ret = Initialize_azClient(&my_client,&azctx);
    if (ret){
        printf("username : %s\ndevice-id : %s\nclient-id : %s\nhost : %s\n",azctx.username,azctx.deviceID,azctx.clientID,azctx.host);
        
    }else{
        printf("az client init failed.");
        
    }
}

// extern int tls_client(char *remoteIPv6);
// extern int tls_server(void);

// normal tcp using gnrc //
extern int tcp_gnrc_native_client(char *remoteIPv6);
extern int tcp_gnrc_native_server(void);
/// posix
extern int tcp_posix_client(char *remoteIPv6);
extern int tcp_posix_server(void);

extern void testSSL(void);


static void *tcp_server_thread(void *arg)
{
    (void)arg;
    tcp_posix_server();
    return NULL; /* should never be reached */
}
static void *mqtt_thread(void *arg)
{
    (void)arg;
    MQTTStartTask(&client);
    return NULL; /* should never be reached */
}
void startMqttTask(void)
{
    thread_create(tls_stack, sizeof(tls_stack), THREAD_PRIORITY_MAIN - 1, 0,
                  mqtt_thread, NULL, "mqttstack");
    return;
}

// static int _cmd_tls_server(int argc, char **argv)
// {
//     (void)argc;
//     (void)argv;
//     thread_create(tls_stack, sizeof(tls_stack), THREAD_PRIORITY_MAIN - 1, 0,
//                   tls_server_thread, NULL, "tlsServer");
//     return 0;
// }

// static int _cmd_tls(int argc, char **argv)
// {
//     if (argc < 2)
//     {
//         printf(
//             "usage: %s <brokerip addr> [port] [clientID] [user] [password] "
//             "[keepalivetime]\n",
//             argv[0]);
//     }

//     tls_client("fec0:affe::100");
//     return 0;
// }

static int _cmd_tcpServer(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    thread_create(tcp_stack, sizeof(tcp_stack), THREAD_PRIORITY_MAIN - 1, 0,
                  tcp_server_thread, NULL, "tcpServer");
    // tcpServer();
    return 0;
}

static int _cmd_tcpClient(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    tcp_posix_client(SERVER_IP);
    return 0;
}

static int _cmd_gnrc_native_tcpServer(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    tcp_gnrc_native_server();
    return 0;
}

static int _cmd_gnrc_native_tcpClient(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    tcp_gnrc_native_client(SERVER_IP);
    return 0;
}



extern int Network_Init(Network *n);
extern int Network_Connect(Network *n, char *remoteIP, int port);
extern void Network_Disconnect(Network *n);
extern int setInitIP(char *selfIP);

static int topic_cnt = 0;
static char _topic_to_subscribe[MAX_TOPICS][MAX_LEN_TOPIC];

static unsigned get_qos(const char *str)
{
    int qos = atoi(str);

    switch (qos)
    {
    case 1:
        return QOS1;
    case 2:
        return QOS2;
    default:
        return QOS0;
    }
}

static void _on_msg_received(MessageData *data)
{
    printf("paho_mqtt_example: message received on topic"
           " %.*s: %.*s\n",
           (int)data->topicName->lenstring.len,
           data->topicName->lenstring.data, (int)data->message->payloadlen,
           (char *)data->message->payload);
}

static int _cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    topic_cnt = 0;
    int res = MQTTDisconnect(&client);
    if (res < 0)
    {
        printf("mqtt_example: Unable to disconnect\n");
    }
    else
    {
        printf("mqtt_example: Disconnect successful\n");
    }

    Network_Disconnect(&network);
    return res;
}
static int _cmd_pub(int argc, char **argv);
static int _cmd_con(int argc, char **argv)
{
    if (argc < 2)
    {
        printf(
            "usage: %s <brokerip addr> [port] [clientID] [user] [password] "
            "[keepalivetime]\n",
            argv[0]);
        return 1;
    }

    //  char *remote_ip = "20.49.110.129";
    char *remote_ip = azctx.host;

    int ret = -1;

    /* ensure client isn't connected in case of a new connection */
    if (client.isconnected)
    {
        printf("mqtt_example: client already connected, disconnecting it\n");
        MQTTDisconnect(&client);
        Network_Disconnect(&network);
    }

    int port = DEFAULT_MQTT_PORT;
    if (argc > 2)
    {
        port = atoi(argv[2]);
    }

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    
    data.MQTTVersion = MQTT_VERSION_v311;

    data.clientID.cstring = DEFAULT_MQTT_CLIENT_ID;
    if (argc > 3)
    {
        data.clientID.cstring = argv[3];
    }

    data.username.cstring = DEFAULT_MQTT_USER;
    
    if (argc > 4)
    {
        data.username.cstring = argv[4];
    }

    data.password.cstring = DEFAULT_MQTT_PWD;
    if (argc > 5)
    {
        data.password.cstring = argv[5];
    }

    data.keepAliveInterval = DEFAULT_KEEPALIVE_SEC;
    if (argc > 6)
    {
        data.keepAliveInterval = atoi(argv[6]);
    }
    //
    data.username.cstring = azctx.username;
    data.clientID.cstring = azctx.clientID;
    //
    data.cleansession = IS_CLEAN_SESSION;
    data.willFlag = 0;
    printf("mqtt_example: Connecting to MQTT Broker from %s %d\n",
           remote_ip, port);
    printf("mqtt_example: Trying to connect to %s, port: %d\n",
           remote_ip, port);
    ret = Network_Connect(&network, remote_ip, port);
    if (ret < 0)
    {
        printf("mqtt_example: Unable to connect with return code %d\n", ret);
        return ret;
    }

    printf("user:%s clientId:%s password:%s\n", data.username.cstring,
           data.clientID.cstring, data.password.cstring);
    // startMqttTask();
    // testSSL();
    MQTTStartTask(&client);
    ret = MQTTConnect(&client, &data);
    if (ret < 0)
    {
        printf("mqtt_example: Unable to connect client %d\n", ret);
        _cmd_discon(0, NULL);
        return ret;
    }
    else
    {
        printf("mqtt_example: Connection successfully\n");
    }

    char *input[] = {"pub", "testt", "hi riot os"};
    _cmd_pub(3, input);
    return (ret > 0) ? 0 : 1;
}

static int _cmd_pub(int argc, char **argv)
{
    enum QoS qos = QOS0;

    if (argc < 3)
    {
        printf("usage: %s <topic name> <string msg> [QoS level]\n",
               argv[0]);
        return 1;
    }
    if (argc == 4)
    {
        qos = get_qos(argv[3]);
    }
    MQTTMessage message;
    message.qos = qos;
    message.retained = IS_RETAINED_MSG;
    message.payload = argv[2];
    message.payloadlen = strlen(message.payload);
    //testSSL();
    int rc;
    if ((rc = MQTTPublish(&client, argv[1], &message)) < 0)
    {
        printf("mqtt_example: Unable to publish (%d)\n", rc);
    }
    else
    {
        printf("mqtt_example: Message (%s) has been published to topic %s"
               "with QOS %d\n",
               (char *)message.payload, argv[1], (int)message.qos);
    }

    return rc;
}

static int _cmd_sub(int argc, char **argv)
{
    enum QoS qos = QOS0;

    if (argc < 2)
    {
        printf("usage: %s <topic name> [QoS level]\n", argv[0]);
        return 1;
    }

    if (argc >= 3)
    {
        qos = get_qos(argv[2]);
    }

    if (topic_cnt > MAX_TOPICS)
    {
        printf("mqtt_example: Already subscribed to max %d topics,"
               "call 'unsub' command\n",
               topic_cnt);
        return -1;
    }

    if (strlen(argv[1]) > MAX_LEN_TOPIC)
    {
        printf("mqtt_example: Not subscribing, topic too long %s\n", argv[1]);
        return -1;
    }
    strncpy(_topic_to_subscribe[topic_cnt], argv[1], strlen(argv[1]));

    printf("mqtt_example: Subscribing to %s\n", _topic_to_subscribe[topic_cnt]);
    int ret = MQTTSubscribe(&client,
                            _topic_to_subscribe[topic_cnt], qos, _on_msg_received);
    if (ret < 0)
    {
        printf("mqtt_example: Unable to subscribe to %s (%d)\n",
               _topic_to_subscribe[topic_cnt], ret);
        _cmd_discon(0, NULL);
    }
    else
    {
        printf("mqtt_example: Now subscribed to %s, QOS %d\n",
               argv[1], (int)qos);
        topic_cnt++;
    }
    return ret;
}

static int _cmd_unsub(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage %s <topic name>\n", argv[0]);
        return 1;
    }

    int ret = MQTTUnsubscribe(&client, argv[1]);

    if (ret < 0)
    {
        printf("mqtt_example: Unable to unsubscribe from topic: %s\n", argv[1]);
        _cmd_discon(0, NULL);
    }
    else
    {
        printf("mqtt_example: Unsubscribed from topic:%s\n", argv[1]);
        topic_cnt--;
    }
    return ret;
}

static unsigned char writebuf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];

static const shell_command_t shell_commands[] =
    {

        {"con", "connect to MQTT broker", _cmd_con},
        {"discon", "disconnect from the current broker", _cmd_discon},
        {"pub", "publish something", _cmd_pub},
        {"sub", "subscribe topic", _cmd_sub},
        {"unsub", "unsubscribe from topic", _cmd_unsub},
        // {"tlsc", "tls client", _cmd_tls},
        // {"tlss", "tls posix server", _cmd_tls_server},
        {"tcps", "tcp posix server", _cmd_tcpServer},
        {"tcpc", "tcp client", _cmd_tcpClient},
        {"n_ts", "native tcp server(works fine)", _cmd_gnrc_native_tcpServer},
        {"n_tc", "ative tcp client(works fine)", _cmd_gnrc_native_tcpClient},
        {NULL, NULL, NULL}};


static void initNetIf(){
   
}

int main(void)
{

    // if (IS_USED(MODULE_GNRC_ICMPV6_ECHO))
    // {
        msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    // }
    setInitIP("192.168.0.95");
     InitializeAZ();
    // ztimer_init();
    Network_Init(&network);
    printf("Hi mqtts\n");
    
    MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, writebuf, BUF_SIZE,
                   readbuf,
                   BUF_SIZE);
    printf("Running mqtt paho example. Type help for commands info\n");

   

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
