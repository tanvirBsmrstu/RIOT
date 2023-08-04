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
    sock_tcp_t *sock; /**< socket number */
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

WOLFSSL_CTX *init_wolfSSL_server(Network *myctx);
WOLFSSL_CTX *init_wolfSSL_client(Network *myctx);
int my_io_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int my_io_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);

//



int tls_server(void)
{
    sock_tcp_queue_t queue;
    sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = TLS_PORT;

    if (sock_tcp_listen(&queue, &local, sock_queue_TLS, SOCK_QUEUE_TLS_LEN, 0) < 0)
    {
        puts("Error creating listening queue");
        return 1;
    }
    puts("This is the TLS Server!");
    printf("Server is listening on port %d\n", TLS_PORT);

    sock_tcp_t *sock;
    puts("server is waiting for incoming request....");
    if (sock_tcp_accept(&queue, &myctx.sock, SOCK_NO_TIMEOUT) < 0)
    {
        puts("Error accepting new sock");
    }

    puts("TCP connection is done\nIniting wolfssl");
    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;
    ctx = init_wolfSSL_server(&myctx);
    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        printf("SSL object creation failed\n");
        sock_tcp_disconnect(myctx.sock);
        sock_tcp_stop_listen(&queue);
        return -1;
    }
    else
    {
        printf("SSL object has been create successfully\n");
    }
    puts("Server will perform TLS handshake");
    // / Perform the TLS handshake
    wolfSSL_SetIOReadCtx(ssl, &myctx);
    wolfSSL_SetIOWriteCtx(ssl, &myctx);
    int ret = wolfSSL_accept(ssl);
    if (ret != SSL_SUCCESS)
    {
        printf("TLS handshake error: %d\n", ret);
    }
    puts("TLS handshake successfull");
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
            serverBuf[0] = '$';
            serverBuf[1] = '$';

            // Echo back to the client
            wolfSSL_write(ssl, serverBuf, bytesRead);
        }
    } while (bytesRead > 0);

    sock_tcp_disconnect(myctx.sock);

    sock_tcp_stop_listen(&queue);
    return 0;
}

int tls_client(char *remoteIP)
{
    int res;
    sock_tcp_t sock;
    sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;

    remote.port = TLS_PORT;
    uint8_t buf[128];
    puts("TLS client");
    Network myctx;
    myctx.sock = &sock;
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, remoteIP);
    puts("TLS client");
    if (sock_tcp_connect(myctx.sock, &remote, 0, 0) < 0)
    {
        puts("Error connecting sock");
        return 1;
    }

    puts("TCP connection is done\nIniting wolfssl");
    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;
    ctx = init_wolfSSL_client(&myctx);

    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        printf("SSL object creation failed\n");
        sock_tcp_disconnect(myctx.sock);

        return -1;
    }
    else
    {
        printf("SSL object has been create successfully\n");
    }
    puts("Server will perform TLS handshake");
    wolfSSL_SetIOReadCtx(ssl, &myctx);
    wolfSSL_SetIOWriteCtx(ssl, &myctx);
    /* Perform the SSL/TLS handshake */
    int ret = wolfSSL_connect(ssl);
    if (ret != SSL_SUCCESS)
    {
        fprintf(stderr, "SSL handshake failed: %d\n", wolfSSL_get_error(ssl, ret));
        wolfSSL_free(ssl);
        sock_tcp_disconnect(myctx.sock);
        wolfSSL_CTX_free(ctx);
        wolfSSL_Cleanup();
        return 1;
    }

    /* SSL/TLS handshake successful */
    printf("SSL handshake successful!\n");

    /* Send data to the server */
    const char *message = "Hello, Server!";
    ret = wolfSSL_write(ssl, message, strlen(message));
    if (ret < 0)
    {
        fprintf(stderr, "Error sending data: %d\n", wolfSSL_get_error(ssl, ret));
        wolfSSL_free(ssl);
        sock_tcp_disconnect(myctx.sock);
        wolfSSL_CTX_free(ctx);
        wolfSSL_Cleanup();
        return 1;
    }

    /* Wait for a while to let the server process the data */
    // ztimer_sleep(2);

    /* Receive data from the server */
    int bytesRead = wolfSSL_read(ssl, buf, sizeof(buf) - 1);
    if (bytesRead > 0)
    {
        buf[bytesRead] = '\0';
        printf("Received from server: %s\n", buf);
    }
    else
    {
        fprintf(stderr, "Error receiving data: %d\n", wolfSSL_get_error(ssl, bytesRead));
    }

    /* Clean up */
    wolfSSL_free(ssl);
    sock_tcp_disconnect(myctx.sock);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

    return 0;
}

WOLFSSL_CTX *init_wolfSSL_ctx_util(bool isServer, Network *myctx)
{
    WOLFSSL_CTX *ctx = NULL;
    wolfSSL_Init();

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(isServer ? wolfTLSv1_2_server_method() : wolfTLSv1_2_client_method())) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        return NULL;
    }
    if (isServer)
    {
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
    }
    else
    {
        /* Load client certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
                                           sizeof_ca_cert_der_2048,
                                           SSL_FILETYPE_ASN1) != SSL_SUCCESS)
        {
            fprintf(stderr, "ERROR: failed to load ca buffer\n");
            return NULL;
        }
    }
    /* Set custom I/O callbacks and context */
    wolfSSL_CTX_SetIORecv(ctx, my_io_recv);
    wolfSSL_CTX_SetIOSend(ctx, my_io_send);
   
    return ctx;
}

WOLFSSL_CTX *init_wolfSSL_server(Network *myctx)
{
    WOLFSSL_CTX *ctx = init_wolfSSL_ctx_util(true, myctx);
    return ctx;
}
WOLFSSL_CTX *init_wolfSSL_client(Network *myctx)
{
    WOLFSSL_CTX *ctx = init_wolfSSL_ctx_util(false, myctx);
    /* Disable certificate validation from the client side */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
    return ctx;
}

void printError(char *msg)
{
    printf("[Error] : %s\n", msg);
}

/* Custom I/O send function */
int my_io_send(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    if (ctx == NULL){
        printError("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return -1;
    }
    // casting to get IOReadCtx   
    Network *app_ctx = (Network *)ctx;
    if (!app_ctx || !app_ctx->sock)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
    }
    // Use sock_tcp_write to send data over the socket
    ssize_t bytesSent = sock_tcp_write(app_ctx->sock, buf, sz);

    if (bytesSent >= 0)
    {
        printf("Sent %zd bytes: %s\n", bytesSent, buf);
    }
    else
    {
        printf("Error while sending data: %zd\n", bytesSent);
    }

    return bytesSent;
}
int my_io_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    if (ctx == NULL){
        printError("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return -1;
    }
    // casting to get IOReadCtx   
    Network *app_ctx = (Network *)ctx;
    if (!app_ctx || !app_ctx->sock)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
    }
    // Use sock_tcp_read to receive data from the socket
    ssize_t bytesRead = sock_tcp_read(app_ctx->sock, buf, sz, SOCK_NO_TIMEOUT);
    if (bytesRead > 0)
    {
        printf("Received %zd bytes: %s\n", bytesRead, buf);
    }
    else if (bytesRead == 0)
    {
        // printf("Connection closed by the peer.\n");
    }
    else
    {
        printf("Error while receiving data: %zd\n", bytesRead);
    }

    return bytesRead;
}

