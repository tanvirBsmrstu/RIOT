#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include "wolfssl/certs_test.h"

#include <net/sock/tcp.h>
#include <errno.h>

#include "net/dns.h"
#include "log.h"

#ifdef MODULE_IPV6_ADDR
#include "net/ipv6/addr.h"
#endif
// #ifdef MODULE_IPV4_ADDR
// #include "net/ipv4/addr.h"
// #endif

#include "net/af.h"

#define SOCK_QUEUE_TLS_LEN (1U)

sock_tcp_t sock_queue_TLS[SOCK_QUEUE_TLS_LEN];
uint8_t serverBuf[128];
extern const unsigned char server_cert[];
extern const unsigned char server_key[];
extern unsigned int server_cert_len;
extern unsigned int server_key_len;

typedef struct Network
{
    sock_tcp_t sock; /**< socket number */
    /**
     * @brief read internal function
     */
    // int (*mqttread) (struct Network*, unsigned char*, int, int);
    // /**
    //  * @brief write internal function
    //  */
    // int (*mqttwrite) (struct Network*, unsigned char*, int, int);
} Network;
#define TLS_PORT 1234

WOLFSSL *init_wolfSSL_server(void);

int tls_server(void)
{
    sock_tcp_queue_t queue;
    sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = TLS_PORT;

    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;

    if (sock_tcp_listen(&queue, &local, sock_queue_TLS, SOCK_QUEUE_TLS_LEN, 0) < 0)
    {
        puts("Error creating listening queue");
        return 1;
    }
    puts("Listening on port 12345");
    while (1)
    {
        sock_tcp_t *sock;

        if (sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT) < 0)
        {
            puts("Error accepting new sock");
            continue;
        }
        puts("This is the TLS Server!");
        printf("Server is listening on port %d\n", TLS_PORT);

        if ((ssl = init_wolfSSL_server()) == NULL)
        {
            printf("SSL object creation failed\n");
            sock_tcp_disconnect(sock);
            sock_tcp_stop_listen(&queue);
            return -1;

        }
        else
        {
            printf("SSL object has been create successfully\n");
        }
        // / Perform the TLS handshake
        int ret = wolfSSL_accept(ssl);
        if (ret != SSL_SUCCESS)
        {
            printf("TLS handshake error: %d\n", ret);
        }

        // TLS handshake successful, echo received data back to the client
        char buffer[128];
        int bytesRead;
        do
        {
            bytesRead = wolfSSL_read(ssl, serverBuf, sizeof(serverBuf) - 1);
            if (bytesRead > 0)
            {
                serverBuf[bytesRead] = '\0'; // Null-terminate the received data
                printf("Received: %s\n", serverBuf);

                // Echo back to the client
                wolfSSL_write(ssl, serverBuf, bytesRead);
            }
        } while (bytesRead > 0);

        int read_res = 0;

        puts("Reading data... wolfssl is ready.");
        while (read_res >= 0)
        {
            read_res = sock_tcp_read(sock, &serverBuf, sizeof(serverBuf),
                                     SOCK_NO_TIMEOUT);
            if (read_res <= 0)
            {
                puts("Disconnected");
                break;
            }
            else
            {
                int write_res;
                printf("Read: \"");
                for (int i = 0; i < read_res; i++)
                {
                    printf("%c", serverBuf[i]);
                }
                puts("\"");
                if ((write_res = sock_tcp_write(sock, &serverBuf,
                                                read_res)) < 0)
                {
                    puts("Errored on write, finished server loop");
                    break;
                }
            }
        }
        sock_tcp_disconnect(sock);
    }
    sock_tcp_stop_listen(&queue);
    return 0;
}

WOLFSSL *init_wolfSSL_server(void)
{
    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;
    wolfSSL_Init();

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        return NULL;
    }

    /* Load certificate file for the DTLS server */
    if (wolfSSL_CTX_use_certificate_buffer(ctx, server_cert,
                                           server_cert_len, SSL_FILETYPE_ASN1) != SSL_SUCCESS)
    {
        LOG(LOG_ERROR, "Failed to load certificate from memory.\n");
        return NULL;
    }

    /* Load the private key */
    if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, server_key,
                                          server_key_len, SSL_FILETYPE_ASN1) != SSL_SUCCESS)
    {
        LOG(LOG_ERROR, "Failed to load private key from memory.\n");
        return NULL;
    }

    // Create a wolfSSL object for the connection
    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        printf("SSL object creation failed\n");
    }
    else
    {
        printf("SSL object has been create successfully\n");
    }
    printf("SSL object setting I//O...\n");
    // Set up the file descriptor for WolfSSL object (client socket)
    // wolfSSL_SetIORecv(ssl, sock_tcp_recv);
    // wolfSSL_SetIOSend(ssl, sock_tcp_send);
    // wolfSSL_SetIOCTX(ssl, &client_socket);

    return ssl;
}
