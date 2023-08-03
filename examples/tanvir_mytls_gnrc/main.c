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

#include "net/ipv6/addr.h"

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static char tcp_stack[THREAD_STACKSIZE_DEFAULT];
static char tls_stack[THREAD_STACKSIZE_DEFAULT];

#define SERVER_IP "fec0:affe::100"
#define CLIENT_IP "fec0:affe::99"


extern int tls_client(char *remoteIPv6);
extern int tls_server(void);

// normal tcp using gnrc //
extern int tcp_gnrc_native_client(char *remoteIPv6);
extern int tcp_gnrc_native_server(void);
/// posix
extern int tcp_posix_client(char *remoteIPv6);
extern int tcp_posix_server(void);


static void *tcp_server_thread(void *arg)
{
    (void)arg;
    tcp_posix_server();
    return NULL; /* should never be reached */
}
static void *tls_server_thread(void *arg)
{
    (void)arg;
    tls_server();
    return NULL; /* should never be reached */
}

static int _cmd_tls_server(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    thread_create(tls_stack, sizeof(tls_stack), THREAD_PRIORITY_MAIN - 1, 0,
                  tls_server_thread, NULL, "tlsServer");
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

    tls_client("fec0:affe::100");
    return 0;
}


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




static const shell_command_t shell_commands[] =
    {

        {"tlsc", "tls client", _cmd_tls},
        {"tlss", "tls posix server", _cmd_tls_server},
        {"tcps", "tcp posix server", _cmd_tcpServer},
        {"tcpc", "tcp client", _cmd_tcpClient},
        {"n_ts", "native tcp server(works fine)", _cmd_gnrc_native_tcpServer},
        {"n_tc", "ative tcp client(works fine)", _cmd_gnrc_native_tcpClient},
        {NULL, NULL, NULL}};

int main(void)
{

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

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
