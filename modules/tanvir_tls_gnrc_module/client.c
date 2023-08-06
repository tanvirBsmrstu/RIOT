
#include "tls_opt.h"
#include "tls_api.h"

#include "wolfssl_common.h"

#include <net/sock/tcp.h>

int internal_io_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int internal_io_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);

int tls_client(char *remoteIP)
{
    sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_tcp_t *sock = NULL;
    int ret;
    remote.port = TLS_PORT;
    uint8_t buf[128];

    char *msg, *err_msg;
    sprintf(msg, "TLS client is starting on port %d remote ip : %s", TLS_PORT, remoteIP);
    print_log(msg);

    tls_api_socket_t *api_socket = get_cb_socket_from_appCtx()(get_app_ctx());
    sock = api_socket->socket;
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, remoteIP);
    print_log("TLS client is trying to connect");
    if ((ret = sock_tcp_connect(sock, &remote, 0, 0)) < 0)
    {
        sprintf(msg, "Error connecting to server err code : %d err reason : %s", TLS_PORT, strerror(errno));
        print_error(msg);
        goto cleanup;
    }

    print_log("TCP connection is done\nIniting wolfssl");
    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;
    ctx = init_wolfSSL_client(internal_io_send, internal_io_recv);

    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        print_error("SSL object creation failed\n");
        goto cleanup;
    }
    else
    {
        print_log("SSL object has been create successfully\n");
    }
    print_log("client will perform TLS handshake");
    wolfSSL_SetIOReadCtx(ssl, get_app_ctx());
    wolfSSL_SetIOWriteCtx(ssl, get_app_ctx());
    /* Perform the SSL/TLS handshake */
    ret = wolfSSL_connect(ssl);
    if (ret != SSL_SUCCESS)
    {
        sprintf(err_msg, "SSL handshake failed: %d\n", wolfSSL_get_error(ssl, ret));
        print_error(err_msg);
        goto cleanup;
    }

    /* SSL/TLS handshake successful */
    print_log("SSL handshake successful!\n");

    /* Send data to the server */
    const char *message = "Hello, Server!";
    ret = wolfSSL_write(ssl, message, strlen(message));
    if (ret < 0)
    {
        fprintf(stderr, "Error sending data: %d\n", wolfSSL_get_error(ssl, ret));
        goto cleanup;
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

cleanup:
    if (sock)
        sock_tcp_disconnect(sock);
    if (ctx)
        wolfSSL_CTX_free(ctx);
    if (ssl)
        wolfssl_free(ssl);
    wolfSSL_Cleanup();

    return 0;
}

/* Custom I/O send function */
int internal_io_send(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    if (ctx == NULL)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return -1;
    }
    // casting to get IOReadCtx
    // Network *app_ctx = (Network *)ctx;
    tls_api_socket_t *api_socket = get_cb_socket_from_appCtx()(ctx);
    if (!api_socket || !api_socket->socket)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
    }
    // Use sock_tcp_write to send data over the socket
    ssize_t bytesSent = sock_tcp_write(api_socket->socket, buf, sz);

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
int internal_io_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    if (ctx == NULL)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to ctx=Null.");
        return -1;
    }
    // casting to get IOReadCtx
    tls_api_socket_t *api_socket = get_cb_socket_from_appCtx()(ctx);
    if (!api_socket || !api_socket->socket)
    {
        printError("underlying socket can not be decided by wolfssl I/O callback due to Invalid socket.");
    }
    // Use sock_tcp_read to receive data from the socket
    ssize_t bytesRead = sock_tcp_read(api_socket->socket, buf, sz, SOCK_NO_TIMEOUT);
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
