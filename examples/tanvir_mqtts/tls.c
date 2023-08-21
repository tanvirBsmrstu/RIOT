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

#include <stdio.h>
#include <stdlib.h>

#include "ztimer.h"
#include "paho_mqtt.h"
#include "MQTTClient.h"

#define WRITE_CHUNK_SIZE 128

#define SOCK_QUEUE_TLS_LEN (1U)

// WOLFSSL_SESSION *ssl_session;

// sock_tcp_t sock_queue_TLS[SOCK_QUEUE_TLS_LEN];
// uint8_t serverBuf[128];
extern const unsigned char server_cert[];
extern const unsigned char server_key[];
extern unsigned int server_cert_len;
extern unsigned int server_key_len;

#define SERVER_CERT "./certs/certificates/server.pem"
#define SERVER_KEY "./certs/certificates/server.key"
unsigned char *server_cert_buf;
unsigned char *server_key_buf;
size_t server_cert_buf_len;
size_t server_key_buf_len;
#define CA_CERT "./last_cert/certs/ca.pem"
unsigned char *ca_cert_buf;
size_t ca_cert_buf_len;
void load_cert_buffer(bool isServer);

#define TLS_PORT 1234

WOLFSSL_CTX *init_wolfSSL_server(Network *myctx);
WOLFSSL_CTX *init_wolfSSL_client(Network *myctx);
int my_io_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int my_io_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int tls_connect(Network *myctx, char *remoteIP, int port);
//
WOLFSSL *ssl = NULL;
WOLFSSL_CTX *ctx = NULL;
// sock_tcp_queue_t queue;
// remote;
// mutex_t ssl_session_mutx;
#include "net/gnrc.h"
// #include "net/gnrc/sock.h"

// static int set_socket_receive_timeout(Network *myctx, uint32_t timeout_ms)
// {
//     int32_t rc = -1;
//     gnrc_netapi_msg_t msg;

//     msg.content.ptr = &timeout_ms;
//     msg.content_len = sizeof(uint32_t);

//     if (gnrc_netapi_set(myctx->sock, GNRC_NETAPI_MSG_TYPE_SOCKOPT,
//                         GNRC_NETAPI_SOCKOPT_RCVTIMEO, &msg, sizeof(uint32_t)) < 0) {
//         printf("Error setting socket receive timeout\n");
//     } else {
//         rc = 0;
//     }

//     return rc;
// }

int SessionRecreate(Network *n)
{
    wolfSSL_free(ssl);
    ssl = NULL;
    sock_tcp_disconnect(&n->sock);
    int res;
    // if ((res = sock_tcp_connect(&n->sock, &remote, 0, 0)) < 0)
    // {
    //     printf("Error connecting sock %s\n", strerror(res));
    //     return res;
    // }
    WOLFSSL *localssl = wolfSSL_new(ctx);
    int ret = wolfSSL_connect(localssl);
    if (ret != SSL_SUCCESS)
    {
        char text[100];
        wolfSSL_ERR_error_string(wolfSSL_get_error(localssl, ret), text);
        fprintf(stderr, "SSL handshake failed: %d with %s\n", wolfSSL_get_error(localssl, ret), text);
        // wolfSSL_free(ssl);
        // sock_tcp_disconnect(&myctx->sock);
        // wolfSSL_CTX_free(ctx);
        // wolfSSL_Cleanup();
        return ret;
    }
    ssl = localssl;
    printf("Session recreated\n");

    return 0;
}

int Network_Read(Network *n, unsigned char *buf, int buf_len, int timeout_ms)
{
    if (!ssl)
    {
        printf("Network Read : No ssl object found\n");
        return 0;
    }
    
    ztimer_now_t start_time = ztimer_now(ZTIMER_MSEC);
    ztimer_now_t end_time = start_time + timeout_ms;
    int recvLen = 0;
    int rc = 0;
    //  printf("Network reading len %d timeout %d\n",buf_len - recvLen, timeout_ms);

    while ((recvLen < buf_len) && (ztimer_now(ZTIMER_MSEC) < end_time))
    {
        int chunk_size = buf_len - recvLen;
        rc = wolfSSL_read(ssl, buf + recvLen, chunk_size);
        int err = wolfSSL_get_error(ssl, rc);
        if (err == WOLFSSL_ERROR_WANT_READ)
        {
            // printf("ERROR : WOLFSSL_ERROR_WANT_READ\n");
            ztimer_sleep(ZTIMER_MSEC, 5);
            continue;
        }

        // printf("Network reading recvLen %d buf_len %d ret %d\n",recvLen,buf_len,rc);
        // if (rc == 0)
        // {
        //     printf("ERROR : timeout returns\n");
        //     return 0;
        // }
        if (rc > 0)
        {
            recvLen += rc;
        }
        else if (rc < 0)
        {
            recvLen = rc;
            break;
        }

        // /* Check if the timeout has been exceeded */

        if (ztimer_now(ZTIMER_MSEC) >= end_time)
        {
            break;
        }
    }
    
    return recvLen;
}
int Network_Send(Network *n, unsigned char *buf, int buf_len, int timeout_ms)
{
    if (!ssl)
    {
        // printf("Network Send : No ssl object found\n");
        return 0;
    }
    ztimer_now_t start_time = ztimer_now(ZTIMER_MSEC);
    ztimer_now_t end_time = start_time + timeout_ms;
    int sentLen = 0;
    printf("Network sending ....\n");
    while (sentLen < buf_len)
    {
        int chunk_size = buf_len - sentLen;

        int rc = 0;
        rc = wolfSSL_write(ssl, buf + sentLen, chunk_size);
        int err = wolfSSL_get_error(ssl, rc);
        if (err == WOLFSSL_ERROR_WANT_WRITE)
        {
            printf("ERROR : WOLFSSL_ERROR_WANT_WRITE\n");
        }
        if (rc > 0)
        {
            sentLen += rc;
        }
        else if (rc < 0)
        {
            sentLen = rc;
            break;
        }

        /* Check if the timeout has been exceeded */
        if (ztimer_now(ZTIMER_MSEC) >= end_time)
        {
            break;
        }
    }
    printf("Network Send : %d bytes\n", sentLen);
    return sentLen;
}
int Network_Connect(Network *n, char *remoteIP, int port)
{
    return tls_connect(n, remoteIP, port);
}
int Network_Init(Network *n)
{

    n->mqttread = Network_Read;
    n->mqttwrite = Network_Send;
    // n->disconnect = Network_Disconnect;
    return 0;
}

void cleanup_client(Network *n)
{
    if (ssl)
        wolfSSL_free(ssl);
    if (ctx)
        wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    sock_tcp_disconnect(&n->sock);
}

void Network_Disconnect(Network *n)
{
    cleanup_client(n);
    return;
}

/*void cleanup(Network *n)
{
    if (ssl)
        wolfSSL_free(ssl);
    if (ctx)
        wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    if (n->sock)
        sock_tcp_disconnect(n->sock);
    sock_tcp_stop_listen(&queue);
}*/

void testSSL(void)
{
    if (ssl == NULL)
    {
        printf("No ssl object\n");
        exit(-1);
    }
    else
    {
        printf("ready.......................ssl.......\n");
    }
}

int tls_connect(Network *myctx, char *remoteIP, int port)
{
    int res;

    sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;

    remote.port = port;

    puts("TLS client");

    // myctx.sock = &sock;
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, remoteIP);
    if ((res = sock_tcp_connect(&myctx->sock, &remote, 0, 0)) < 0)
    {
        printf("Error connecting sock %s\n", strerror(res));
        return res;
    }

    puts("TCP connection is done\nIniting wolfssl");
    ctx = init_wolfSSL_client(myctx);
    WOLFSSL *localssl = NULL;
    if ((localssl = wolfSSL_new(ctx)) == NULL)
    {
        printf("SSL object creation failed\n");
        // sock_tcp_disconnect(&myctx->sock);

        return -1;
    }
    else
    {
        printf("SSL object has been create successfully\n");
    }
    puts("Server will perform TLS handshake");
    wolfSSL_SetIOReadCtx(localssl, myctx);
    wolfSSL_SetIOWriteCtx(localssl, myctx);
    /* Perform the SSL/TLS handshake */
    int ret = wolfSSL_connect(localssl);
    if (ret != SSL_SUCCESS)
    {
        char text[100];
        wolfSSL_ERR_error_string(wolfSSL_get_error(localssl, ret), text);
        fprintf(stderr, "SSL handshake failed: %d with %s\n", wolfSSL_get_error(localssl, ret), text);
        // wolfSSL_free(ssl);
        // sock_tcp_disconnect(&myctx->sock);
        // wolfSSL_CTX_free(ctx);
        // wolfSSL_Cleanup();
        return ret;
    }
    ssl = localssl;
    /* SSL/TLS handshake successful */
    printf("SSL handshake successful!\n");

    /* Clean up */

    return 0;
}

WOLFSSL_CTX *init_wolfSSL_ctx_util(bool isServer, Network *myctx)
{
    WOLFSSL_CTX *ctx = NULL;
    wolfSSL_Init();
    load_cert_buffer(isServer);

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(isServer ? wolfTLSv1_2_server_method() : wolfTLSv1_2_client_method())) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        return NULL;
    }
    if (isServer)
    {
        /* Load certificate file for the DTLS server */
        if (wolfSSL_CTX_use_certificate_buffer(ctx, server_cert_buf,
                                               server_cert_buf_len, SSL_FILETYPE_PEM) != SSL_SUCCESS)
        {
            LOG(LOG_ERROR, "Failed to load certificate from memory.\n");
            return NULL;
        }

        /* Load the private key */
        if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, server_key_buf,
                                              server_key_buf_len, SSL_FILETYPE_PEM) != SSL_SUCCESS)
        {
            LOG(LOG_ERROR, "Failed to load private key from memory.\n");
            return NULL;
        }
    }
    else
    {
        /* Load client certificates into WOLFSSL_CTX */
        if (wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_buf,
                                           ca_cert_buf_len,
                                           SSL_FILETYPE_PEM) != SSL_SUCCESS)
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
    if (ctx == NULL)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return -1;
    }
    // casting to get IOReadCtx
    Network *app_ctx = (Network *)ctx;
    if (!app_ctx)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
    }
    // Use sock_tcp_write to send data over the socket
    ssize_t bytesSent = sock_tcp_write(&app_ctx->sock, buf, sz);

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
    if (ctx == NULL)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return -1;
    }
    // casting to get IOReadCtx
    Network *app_ctx = (Network *)ctx;
    if (!app_ctx)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
    }
    // Use sock_tcp_read to receive data from the socket
    ssize_t bytesRead = sock_tcp_read(&app_ctx->sock, buf, sz, 0);

    if (bytesRead > 0)
    {
        printf("Received %zd bytes: %s\n", bytesRead, buf);
    }
    else if (bytesRead == 0)
    {
        printf("Connection closed by the peer.\n");
    }
    else
    {
        switch (bytesRead)
        {
        case -ETIMEDOUT:
        case -EAGAIN:
            // printf("Reading timeout...\n");
            return -323;
            break;

        default:
            printf("Error while receiving data: %zd\n", bytesRead);
            break;
        }
    }

    return bytesRead;
}

////////////////////////////////////////////////////////////

unsigned char *read_file_to_buffer(const char *filename, size_t *buffer_size)
{
    FILE *file = fopen(filename, "rb"); // Open the file in binary read mode

    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (!buffer)
    {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size)
    {
        perror("Failed to read file");
        free(buffer);
        return NULL;
    }

    if (buffer_size)
    {
        *buffer_size = (size_t)file_size;
    }

    return buffer;
}

void load_cert_buffer(bool isServer)
{
    if (isServer)
    {
        server_cert_buf = read_file_to_buffer(SERVER_CERT, &server_cert_buf_len);
        server_key_buf = read_file_to_buffer(SERVER_KEY, &server_key_buf_len);
    }
    else
    {
        ca_cert_buf = read_file_to_buffer(CA_CERT, &ca_cert_buf_len);
    }

    return;
}
///////////////////////////////////////////

/*
int tls_server(void)
{

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
    ctx = NULL;
    ssl = NULL;
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
    return 0;
}
*/
