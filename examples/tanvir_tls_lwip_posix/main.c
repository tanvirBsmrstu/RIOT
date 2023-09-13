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

#include "lwip/ip4_addr.h"
#include "net/ipv4/addr.h"
#include "lwip.h"
#include "lwip/opt.h"
#include "lwip/netif.h"

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int tls_client(char *serverIP);
extern int tls_server(void);
extern int tcp_client(char *serverIP);
extern int tcp_server(void);
// extern int tclient(void);

#define SERVER_IP "192.168.1.5"
#define CLIENT_IP "192.168.1.7"


static char stack_tls_server[THREAD_STACKSIZE_DEFAULT];
static char stack_tcp_server[THREAD_STACKSIZE_DEFAULT];
static void *tls__server_thread(void *arg)
{
    (void)arg;
    tls_server();
    return NULL; /* should never be reached */
}
static void *tcp_server_thread(void *arg)
{
    (void)arg;
    tcp_server();
    return NULL; /* should never be reached */
}
static int _cmd_tls_server(int argc, char **argv)
{
    //(void)argc;(void)argv;

    // tls_server(argc, argv);
    thread_create(stack_tls_server, sizeof(stack_tls_server), THREAD_PRIORITY_MAIN - 1, 0,
                  tls__server_thread, NULL, "TlsServer");
    return 0;
}

static int _cmd_tls(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    tls_client(SERVER_IP);
    return 0;
}
static int _cmd_tcp_server(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    thread_create(stack_tcp_server, sizeof(stack_tcp_server), THREAD_PRIORITY_MAIN - 1, 0,
                  tcp_server_thread, NULL, "tcpServer");

    return 0;
}

static int _cmd_tcp(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    tcp_client(SERVER_IP);
    // tclient();
    return 0;
}
// static void manual_eth_address(struct netif *iface, char *ip)
// {
//     ip4_addr_t ipaddr, netmask, gw;

//     if (ip4addr_aton(ip, &ipaddr) == 0)
//     {
//         puts("Error: unable to parse destination address");
//         return;
//     }

//     netif_add_ip4_address(iface, &ipaddr, 0);
//     printf("ip set to %s\n", ip);
// }
static void manual_eth_address(struct netif *iface,char* ip)
{

     int octets[4];
    int i = 0,token=0,index=0;
    for(;ip[i];i++){
        if(ip[i]=='.'){
            octets[index++]=token;
            token=0;
            continue;
        }
        token=(ip[i]-'0')+10*token;
    }
    octets[index]=token;

    // while ((token = strtok_r(ip, ".", &ip))) {
    //     octets[i++] = atoi(token);
    // }

    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr, octets[0], octets[1],octets[2], octets[3]);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw,octets[0], octets[1],octets[2], 1);
    netif_set_addr(iface, &ipaddr, &netmask, &gw);
     printf("ip set to %s\n", ip);
}

static int difconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next)
    {
        printf("%s_%02u: \n", iface->name, iface->num);

        // char addrstr[IPV6_ADDR_MAX_STR_LEN];
        // for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++)
        // {
        //     if (!ipv6_addr_is_unspecified((ipv6_addr_t *)&iface->ip6_addr[i]))
        //     {
        //         printf(" inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&iface->ip6_addr[i],
        //                                                sizeof(addrstr)));
        //     }
        // }

        puts("");
    }
    if (argc > 1)
    {
        char *ip;
        if (strncmp(argv[1], "server", 6) == 0)
        {
            ip = SERVER_IP;
        }
        else if (strncmp(argv[1], "client", 6) == 0)
        {
            ip = CLIENT_IP;
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
        {"tlsc", "tls client", _cmd_tls},
        {"tlss", "tls server start", _cmd_tls_server},
        {"ip", "ip link", difconfig},
        {"tcps", "tcp server", _cmd_tcp_server},
        {"tcpc", "tcp client", _cmd_tcp},
        {NULL, NULL, NULL}};

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

int main(void)
{

    // msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    //     }
    #ifdef MODULE_LWIP
        /* let LWIP initialize */
        /* Initialize TCP/IP stack */
        printf("LWIP\n");
          ztimer_init();
          lwip_bootstrap();
         ztimer_sleep(ZTIMER_MSEC, 5);
    #endif

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
