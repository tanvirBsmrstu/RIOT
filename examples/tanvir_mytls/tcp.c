

#include <stdio.h>
// #include "net/sock/tcp.h"
// #include <wolfssl/wolfcrypt/settings.h>
// #include <wolfssl/ssl.h>
// #include "wolfssl/certs_test.h"
// #include <arpa/inet.h>
// #include <netinet/in.h>

// #include "net/af.h"

/* socket includes */
#include <sys/socket.h>
#include "net/ipv6/addr.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "lwip/opt.h"

// #include <wolfssl/options.h>

#define TCP_PORT 1211
#define CERT_FILE "../certs/server-cert.pem"

#define SOCK_QUEUE_LEN (5U)

// sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
uint8_t buf[128];

int tcp_server(int argc, char **argv)
{
    int listen_sock, conn_sock; // listening socket, connection socket
    char buff[256];
    int shutdown = 0;
    /* declare wolfSSL objects */

    struct sockaddr_in6 servAddr;

    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((listen_sock = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        return -1;
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));
    /* Fill in the server address */
    servAddr.sin6_family = AF_INET6;      /* using IPv6      */
    servAddr.sin6_port = htons(TCP_PORT); /* on DEFAULT_PORT */

    (void)argc;
    (void)argv;

    /* Bind the server socket to local port */
    if (bind(listen_sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        fprintf(stderr, "ERROR: failed to bind\n");
        return -1;
    }

    /* Listen for a new connection, allow 5 pending connections */
    if (listen(listen_sock, 5) == -1)
    {
        fprintf(stderr, "ERROR: failed to listen\n");
        return -1;
    }

    puts("This is the TCP Server!");
    printf("Server is listening on port %d\n", TCP_PORT);

    struct sockaddr_in6 clientAddr;
    socklen_t size = sizeof(clientAddr);
    /* Continue to accept clients until shutdown is issued */
    while (!shutdown) {
        
        size_t len;
        printf("Waiting for a connection...\n");

        /* Accept client connections */
        int conn_sock = accept(listen_sock, (struct sockaddr *)&clientAddr, &size);
        if (conn_sock < 0)  {
            fprintf(stderr, "ERROR: failed to accept the connection\n\n");
            continue;
        }
        printf("Client connected successfully\n");

        /* Read the client data into our buff array */
        memset(buff, 0, sizeof(buff));
        if (read(conn_sock, buff, sizeof(buff)-1) == -1) {
            fprintf(stderr, "ERROR: failed to read\n");
            return -1;
        }

        /* Print to stdout any data the client sends */
        printf("<-Client sent a message!\n");
        printf("Client said: %s\n", buff);

        /* Check for server shutdown command */
        if (strncmp(buff, "shutdown", 8) == 0) {
            printf("Shutdown command issued!\n");
            shutdown = 1;
        }

        /* Write our reply into buff */
        strncpy(buff, "I hear ya fa shizzle!\n", sizeof(buff) - 1);
        len = strnlen(buff, sizeof(buff));
        printf("Sending reply to client, reply reads: %s\n", buff);

        /* Reply back to the client */
        if (write(conn_sock, buff, len) != (int) len) {
            fprintf(stderr, "ERROR: failed to write\n");
            return -1;
        }

        /* Cleanup after this connection */
        close(conn_sock);           /* Close the connection to the client   */
    }

    printf("Shutdown complete\n");

    /* Cleanup and return */
    close(listen_sock); 

    

  
// int read_res = 0;

// puts("Reading data... wolfssl is ready.");
// while (read_res >= 0)
// {
//     read_res = sock_tcp_read(sockfd, &buf, sizeof(buf),
//                              SOCK_NO_TIMEOUT);
//     if (read_res <= 0)
//     {
//         puts("Disconnected");
//         break;
//     }
//     else
//     {
//         int write_res;
//         printf("Read: \"");
//         for (int i = 0; i < read_res; i++)
//         {
//             printf("%c", buf[i]);
//         }
//         puts("\"");
//         if ((write_res = sock_tcp_write(sockfd, &buf,
//                                         read_res)) < 0)
//         {
//             puts("Errored on write, finished server loop");
//             break;
//         }
//     }
// }

/* Cleanup after this connection */

    printf("Shutdown complete\n");

    ////////////////////////////

    // close(sockfd);
    // ////////////////// clean up /////////////
    // wolfSSL_free(ssl);
    // wolfSSL_CTX_free(ctx);
    // wolfSSL_Cleanup();

    return 0;
}

// int data_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
// {
//     printf("Data arrives : %s\n", buf);
//     return 0;
// }
// int data_send(WOLFSSL *ssl, char *buf, int sz, void *ctx)
// {
//     printf("Data send : %s\n", buf);
//     return 0;
// }

int tcp_client(int argc, char **argv)
{

    int conn_sock = 0;

    char *server_ip = NULL;

    puts("This is a TLS Client!");
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s IP_ADDR_SERVER\n", argv[0]);
        return -1;
    }
    printf("Client is connecting to server at address %s port %d...\n", argv[1], TCP_PORT);
    server_ip = "fec0:affe::100";

    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((conn_sock = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        return -1;
    }
    struct sockaddr_in6 servAddr;
    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));
    /* Fill in the server address */
    servAddr.sin6_family = AF_INET6;      /* using IPv6      */
    servAddr.sin6_port = htons(TCP_PORT); /* on SERVER_PORT */

    /* Get the server IPv6 address from the compile-time string parameter */
    if (inet_pton(AF_INET6, server_ip, &servAddr.sin6_addr.s6_addr) != 1)
    {
        fprintf(stderr, "ERROR: invalid address\n");
        return -1;
    }

    /* Connect to the server */
    if (connect(conn_sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        fprintf(stderr, "ERROR: failed to connect\n");
        return -1;
    }
    printf("TCP connection done with socket: %d\n", conn_sock);
    puts("Sending \"Hello!\"");
    char               buff[] = "Hello, TCP Server!\0";
    printf("Message for server: %s\n", buff);
    int len = strlen(buff);

    /* Send the message to the server */
    if (write(conn_sock, buff, len) != (int) len) {
        fprintf(stderr, "ERROR: failed to write\n");
        return -1;
    }

    /* Read the server data into our buff array */
    memset(buff, 0, sizeof(buff));
    if (read(conn_sock, buff, sizeof(buff)-1) == -1) {
        fprintf(stderr, "ERROR: failed to read\n");
        return -1;
    }

    /* Print to stdout any data the server sends */
    printf("Server sent a reply!\n");
    printf("Server Response was:  %s\n", buff);

    /* Cleanup and exit */
    close(conn_sock);          
    // int res;
    // if ((res = sock_tcp_write(&sock, "Hello!", sizeof("Hello!"))) < 0)
    // {
    //     puts("Errored on write");
    // }
    // else
    // {
    //     if ((res = sock_tcp_read(&sock, &buf, sizeof(buf),
    //                              SOCK_NO_TIMEOUT)) <= 0)
    //     {
    //         puts("Disconnected");
    //     }
    //     printf("Read: \"");
    //     for (int i = 0; i < res; i++)
    //     {
    //         printf("%c", buf[i]);
    //     }
    //     puts("\"");
    // }
    // close(conn_sock);

    return 0;
}
