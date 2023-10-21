
#include "mqtts_riot_iface.h"
#include <gnrc_wolfssl_tls.h>

// mqtts_module.c
#include <stdio.h>
#include "paho_mqtt.h"
#include "MQTTClient.h"

#ifndef MAX_LEN_TOPIC
#define MAX_LEN_TOPIC 64
#endif

#ifndef MAX_TOPICS
#define MAX_TOPICS 8
#endif
#ifndef MQTT_VERSION_v311
#define MQTT_VERSION_v311               4       /* MQTT v3.1.1 version is 4 */
#endif

static int topic_cnt = 0;
static char _topic_to_subscribe[MAX_TOPICS][MAX_LEN_TOPIC];

struct MQTTSContext
{
    TLSContext *tls_ctx;
    MQTTClient *mqtt_client;
    int keep_alive_interval_sec;
    int is_clean_session;
    int is_retained_msg;
    int command_timeout_ms;
};

static int mqtts_initialized = 0;
static Network networkStack;
static MQTTClient client;

static void _on_msg_received(MessageData *data)
{
    printf("paho_mqtt_example: message received on topic"
           " %.*s: %.*s\n",
           (int)data->topicName->lenstring.len,
           data->topicName->lenstring.data, (int)data->message->payloadlen,
           (char *)data->message->payload);
}

int MQTTS_Read(Network *n, unsigned char *buf, int buf_len, int timeout_ms)
{
    return tls_receive(((MQTTSContext *)n->app_context)->tls_ctx, buf, buf_len, timeout_ms);
}
int MQTTS_Write(Network *n, unsigned char *buf, int buf_len, int timeout_ms)
{
    return tls_send(((MQTTSContext *)n->app_context)->tls_ctx, buf, buf_len, timeout_ms);
}
int mqtts_init(MQTTSContext *mqtts_ctx, unsigned char *writebuf, int writebuf_size, unsigned char *readbuf, int readbuf_size)
{
    if (mqtts_initialized)
    {
        tls_log("MQTT-S module already initialized");
        return 0;
    }
    networkStack.mqttread = MQTTS_Read;
    networkStack.mqttwrite = MQTTS_Write;
    networkStack.app_context = mqtts_ctx;

    if (tls_init(mqtts_ctx->tls_ctx) < 0)
    {
        tls_close(mqtts_ctx->tls_ctx);
        return -1;
    }

    // Perform initialization steps if needed

    tls_log("MQTT-S module initialized");

    MQTTClientInit(mqtts_ctx->mqtt_client, &networkStack, mqtts_ctx->command_timeout_ms, writebuf, writebuf_size,
                   readbuf,
                   readbuf_size);

    mqtts_initialized = 1;
    return 0;
}

MQTTSContext *mqtts_create_context(void (*_log_callback)(const char *message))
{

    TLSContext* tls_ctx =  tls_create_context(_log_callback);
    if(tls_ctx==NULL){
        tls_log("tls context creation failed");
        return NULL;
    }
    // if (!mqtts_initialized)
    // {
    //     tls_log("MQTT-S module not initialized");
    //     return NULL;
    // }

    MQTTSContext *ctx = (MQTTSContext *)malloc(sizeof(MQTTSContext));
    if (ctx == NULL)
    {
        tls_log("Failed to allocate memory for MQTT-S context");
        return NULL;
    }

    if (mqtts_set_tls_context(ctx, tls_ctx) < 0)
    {
        free(ctx);
        return NULL;
    }
    ctx->command_timeout_ms = 4000;
    ctx->keep_alive_interval_sec = 240;
    ctx->mqtt_client = &client;

    // Initialize other members of the context

    return ctx;
}

int mqtts_set_tls_context(MQTTSContext *mqtts_ctx, TLSContext *tls_ctx)
{
    if (mqtts_ctx == NULL || tls_ctx == NULL)
    {
        tls_log("Invalid MQTT-S or TLS context");
        return -1;
    }

    mqtts_ctx->tls_ctx = tls_ctx;
    return 0;
}

int mqtts_connect(MQTTSContext *mqtts_ctx, char *remoteAddress, char *clientID, char *username, char *password)
{
    if (mqtts_ctx == NULL)
    {
        tls_log("Invalid MQTT-S context");
        return -1;
    }
    int port = 8883;
    // Perform MQTT connection setup using Paho MQTT and TLS module

    int ret = -1;

    /* ensure client isn't connected in case of a new connection */
    if (mqtts_ctx->mqtt_client->isconnected)
    {
        printf("mqtt_example: client already connected, disconnecting it\n");
    }

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    data.MQTTVersion = MQTT_VERSION_v311;

    data.keepAliveInterval = mqtts_ctx->keep_alive_interval_sec;
    data.cleansession = mqtts_ctx->is_clean_session;
    data.willFlag = 0;
    //
    data.username.cstring = username;
    data.clientID.cstring = clientID;
    data.password.cstring = password;
    //

    printf("mqtt_example: Connecting to MQTT Broker from %s %d\n",
           remoteAddress, port);
    printf("mqtt_example: Trying to connect userName %s, client id : %s pass : %s\n",
           data.username.cstring, data.clientID.cstring, data.password.cstring);
    ret = tls_establish_connection(remoteAddress, port, mqtts_ctx->tls_ctx);
    if (ret < 0)
    {
        printf("mqtt_example: Unable to connect with return code %d\n", ret);
        tls_close(mqtts_ctx->tls_ctx);
        return ret;
    }
    //  startMqttTask();
    printf("starting mqtt task.......");
    MQTTStartTask(mqtts_ctx->mqtt_client);
    printf("trying mqtt connect.......");
    ret = MQTTConnect(mqtts_ctx->mqtt_client, &data);
    if (ret < 0)
    {
        printf("mqtt_example: Unable to connect client %d\n", ret);
        mqtts_disconnect(mqtts_ctx);
        return ret;
    }
    else
    {
        printf("mqtt_example: Connection successfully (%d) ret :%d\n", mqtts_ctx->mqtt_client->isconnected, ret);
    }

    return ret;
}

int mqtts_publish(MQTTSContext *mqtts_ctx, const char *topic, char *payload, int qos, int retained)
{
    if (mqtts_ctx == NULL)
    {
        tls_log("Invalid MQTT-S context");
        return -1;
    }

    // Perform MQTT publish using Paho MQTT and TLS module
    MQTTMessage message;
    message.qos = qos;
    message.retained = mqtts_ctx->is_retained_msg;
    message.payload = payload;
    message.payloadlen = strlen(message.payload);
    int rc;
    if ((rc = MQTTPublish(mqtts_ctx->mqtt_client, topic, &message)) < 0)
    {
        printf("mqtt_example: Unable to publish (%d)\n", rc);
    }
    else
    {
        printf("mqtt_example: Message (%s) has been published to topic %s"
               "with QOS %d\n",
               (char *)message.payload, topic, (int)message.qos);
    }

    return rc;
}

int mqtts_subscribe(MQTTSContext *mqtts_ctx, const char *topic, int qos)
{
    if (mqtts_ctx == NULL)
    {
        tls_log("Invalid MQTT-S context");
        return -1;
    }

    // Perform MQTT subscribe using Paho MQTT and TLS module

    if (topic_cnt > MAX_TOPICS)
    {
        printf("mqtt_example: Already subscribed to max %d topics,"
               "call 'unsub' command\n",
               topic_cnt);
        return -1;
    }

    if (strlen(topic) > MAX_LEN_TOPIC)
    {
        printf("mqtt_example: Not subscribing, topic too long %s\n", topic);
        return -1;
    }
    strncpy(_topic_to_subscribe[topic_cnt], topic, strlen(topic));

    printf("mqtt_example: Subscribing to %s\n", _topic_to_subscribe[topic_cnt]);
    int ret = MQTTSubscribe(mqtts_ctx->mqtt_client,
                            _topic_to_subscribe[topic_cnt], qos, _on_msg_received);
    if (ret < 0)
    {
        printf("mqtt_example: Unable to subscribe to %s (%d)\n",
               _topic_to_subscribe[topic_cnt], ret);
        mqtts_disconnect(mqtts_ctx);
    }
    else
    {
        printf("mqtt_example: Now subscribed to %s, QOS %d\n",
               topic, (int)qos);
        topic_cnt++;
    }
    return ret;
}
int mqtts_unsubscribe(MQTTSContext *mqtts_ctx, const char *topic)
{
    if (mqtts_ctx == NULL)
    {
        tls_log("Invalid MQTT-S context");
        return -1;
    }

    // Perform MQTT subscribe using Paho MQTT and TLS module
    int ret = MQTTUnsubscribe(mqtts_ctx->mqtt_client, topic);

    if (ret < 0)
    {
        printf("mqtt_example: Unable to unsubscribe from topic: %s\n", topic);
        mqtts_disconnect(mqtts_ctx);
    }
    else
    {
        printf("mqtt_example: Unsubscribed from topic:%s\n", topic);
        topic_cnt--;
    }
    return ret;
}
void mqtts_disconnect(MQTTSContext *mqtts_ctx)
{
    if (mqtts_ctx == NULL)
    {
        tls_log("Invalid MQTT-S context");
        return;
    }

    // Perform MQTT disconnection using Paho MQTT and TLS module
    topic_cnt = 0;
    int res = 0;
    if (mqtts_ctx->mqtt_client->isconnected == false)
        goto close_tls_layer;
    res = MQTTDisconnect(mqtts_ctx->mqtt_client);
    if (res < 0)
    {
        printf("mqtt_example: Unable to disconnect\n");
    }
    else
    {
        printf("mqtt_example: Disconnect successful\n");
    }
close_tls_layer:
    // tls_close(mqtts_ctx->tls_ctx);
    // Free allocated resources

    // free(mqtts_ctx);
    mqtts_cleanup();
    return;
}

void mqtts_cleanup(void)
{
    if (!mqtts_initialized)
    {
        tls_log("MQTT-S module not initialized");
        return;
    }

    // Perform cleanup steps if needed

    tls_log("MQTT-S module cleaned up");

    mqtts_initialized = 0;
}
int mqtts_set_certificate(MQTTSContext *mqtts_ctx, const char *ca_cert_path, const char *cert_chain_path, const char *private_key_path)
{
    if (mqtts_ctx == NULL)
    {
        tls_log("Invalid MQTT-S context");
        return -1;
    }
    return tls_set_certificate(mqtts_ctx->tls_ctx, ca_cert_path, cert_chain_path, private_key_path);
}
