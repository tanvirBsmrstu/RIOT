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
#include "paho_mqtt.h"
#include "MQTTClient.h"

#include "lwip/ip6_addr.h"
#include "net/ipv6/addr.h"
#include "lwip.h"
#include "lwip/opt.h"
#include "lwip/netif.h"

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define BUF_SIZE 1024
#define MQTT_VERSION_v311 4 /* MQTT v3.1.1 version is 4 */
#define COMMAND_TIMEOUT_MS 4000

#ifndef DEFAULT_MQTT_CLIENT_ID
#define DEFAULT_MQTT_CLIENT_ID "thasan"
#endif

#ifndef DEFAULT_MQTT_USER
#define DEFAULT_MQTT_USER "thasan"
#endif

#ifndef DEFAULT_MQTT_PWD
#define DEFAULT_MQTT_PWD "thasan"
#endif

/**
 * @brief Default MQTT port
 */
#define DEFAULT_MQTT_PORT 1883

/**
 * @brief Keepalive timeout in seconds
 */
#define DEFAULT_KEEPALIVE_SEC 10

#ifndef MAX_LEN_TOPIC
#define MAX_LEN_TOPIC 100
#endif

#ifndef MAX_TOPICS
#define MAX_TOPICS 4
#endif

#define IS_CLEAN_SESSION 1
#define IS_RETAINED_MSG 0

static int topic_cnt = 0;
static char _topic_to_subscribe[MAX_TOPICS][MAX_LEN_TOPIC];

extern int tls_client(int argc, char **argv);
extern int tls_server(void);
extern int tcp_client(int argc, char **argv);
extern int tcp_server(int argc, char **argv);
// extern int tclient(void);

static char stack[THREAD_STACKSIZE_DEFAULT];
static void *tls_thread(void *arg)
{
    (void)arg;
    tls_server();
    return NULL;    /* should never be reached */
}
static int _cmd_tls_server(int argc, char **argv)
{
    //(void)argc;(void)argv;

    // tls_server(argc, argv);
    thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN-1, 0,
                      tls_thread, NULL, "tcpServer");
    return 0;
}


static int _cmd_tls(int argc, char **argv)
{
    if (argc < 2)
    {
        printf(
            "usage: %s <brokerip addr> [port] [clientID] [user] [password] "
            "[keepalivetime]\n",
            argv[0]);
    }

    tls_client(argc, argv);
    return 0;
}
static int _cmd_tcp_server(int argc, char **argv)
{
    //(void)argc;(void)argv;

    tcp_server(argc, argv);
    return 0;
}

static int _cmd_tcp(int argc, char **argv)
{
    if (argc < 2)
    {
        printf(
            "usage: %s <brokerip addr> [port] [clientID] [user] [password] "
            "[keepalivetime]\n",
            argv[0]);
    }

    tcp_client(argc, argv);
    // tclient();
    return 0;
}
static void manual_eth_address(struct netif *iface, char *ip)
{
    ip6_addr_t ipaddr, netmask, gw;

    if (ip6addr_aton(ip, &ipaddr) == 0)
    {
        puts("Error: unable to parse destination address");
        return;
    }

    netif_add_ip6_address(iface, &ipaddr, 0);
    printf("ip set to %s\n", ip);
}

static int difconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next)
    {
        printf("%s_%02u: ", iface->name, iface->num);

        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++)
        {
            if (!ipv6_addr_is_unspecified((ipv6_addr_t *)&iface->ip6_addr[i]))
            {
                printf(" inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&iface->ip6_addr[i],
                                                       sizeof(addrstr)));
            }
        }

        puts("");
    }
    if (argc > 1)
    {
        char *ip;
        if (strncmp(argv[1], "server", 6) == 0)
        {
            ip = "fec0:affe::100";
        }
        else if (strncmp(argv[1], "client", 6) == 0)
        {
            ip = "fec0:affe::99";
        }
        else
        {
            printf("Wrong ip\n");
            return 0;
        }

        for (struct netif *iface = netif_list; iface != NULL; iface = iface->next)
        {
            // if (strncmp(iface->name, "lo", 2) != 0)
            //     /* Add site-local address */
            //     add_site_local_address(iface);
            if (strncmp(iface->name, "ET", 2) == 0)
                manual_eth_address(iface, ip);
            /* IPv4: eth address */
        }
    }
    return 0;
}

static const shell_command_t shell_commands[] =
    {
        {"mtls", "tls from topic", _cmd_tls},
        {"server", "tls server start", _cmd_tls_server},
        {"ip", "ip link", difconfig},
        {"tcps", "tcp server", _cmd_tcp_server},
        {"tcpc", "tls from topic", _cmd_tcp},
        {NULL, NULL, NULL}};

static unsigned char buf[BUF_SIZE];
static unsigned char readbuf[BUF_SIZE];

// static char azMsg[] =
// static void add_site_local_address(struct netif *iface)
// {
//     (void)iface;
// #ifdef MODULE_LWIP_IPV6
//     #define SITE_LOCAL_PREFIX 0xBBAAC0FE
//     ip6_addr_t sl_addr;
//     memcpy(&sl_addr, &iface->ip6_addr[0], sizeof(ip6_addr_t));
//     sl_addr.addr[0] = SITE_LOCAL_PREFIX;
//     netif_add_ip6_address(iface, &sl_addr, NULL);
// #endif
// }

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{

    // msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    //     }
    // #ifdef MODULE_LWIP
    //     /* let LWIP initialize */
    //     /* Initialize TCP/IP stack */
    //     printf("LWIP\n");
    //       ztimer_init();
    //       lwip_bootstrap();
    //      ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    // #endif

    // NetworkInit(&network);

    // MQTTClientInit(&client, &network, COMMAND_TIMEOUT_MS, buf, BUF_SIZE,
    //                readbuf,
    //                BUF_SIZE);
    // printf("Running mqtt paho example. Type help for commands info\n");

    // MQTTStartTask(&client);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
