
#include "tls_opt.h"
#include "tls_api.h"

#include "wolfssl_common.h"

#include <net/sock/tcp.h>

#define SOCK_QUEUE_TLS_LEN (1U)

sock_tcp_t sock_queue_TLS[SOCK_QUEUE_TLS_LEN];

int internal_io_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx);
int internal_io_send(WOLFSSL *ssl, char *buf, int sz, void *ctx);

int tls_server()
{
    sock_tcp_queue_t queue;
    sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = TLS_PORT;

    char *err_msg = NULL;
    char *msg = NULL;
    int return_code = 0;

    sprintf(msg, "TLS server is starting on port %d\n", TLS_PORT);

    print_log(msg);

    if ((return_code = sock_tcp_listen(&queue, &local, sock_queue_TLS, SOCK_QUEUE_TLS_LEN, 0)) < 0)
    {
        sprintf(err_msg, "Error creating tcp listening queue : code=%d msg= %s\n", return_code, strerror(return_code));
        print_error(err_msg);
        return -1;
    }
    print_log("TLS Server is waiting for incoming connection...");
    app_ctx_t _app_ctx = get_app_ctx();
    sock_tcp_t *sock = get_cb_socket_from_appCtx()(_app_ctx)->socket;

    if ((return_code = sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT)) < 0)
    {
        sprintf(err_msg, "Error accepting new sock : code=%d msg= %s\n", return_code, strerror(return_code));
        print_error(err_msg);
        goto cleanup;
    }

    print_log("TCP connection is done\nIniting wolfssl");

    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;
    ctx = init_wolfSSL_server(internal_io_send, internal_io_recv);
    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        print_error("Error SSL object creation");
    }
    print_log("wolfSSL object has been create successfully");
    print_log("Setting wolfssl I/O r/w context");

    wolfSSL_SetIOReadCtx(ssl, _app_ctx);
    wolfSSL_SetIOWriteCtx(ssl, _app_ctx);

    print_log("Will perform TLS handshake");
    // / Perform the TLS handshake
    return_code = wolfSSL_accept(ssl);
    if (return_code != SSL_SUCCESS)
    {
        sprintf(err_msg, "Error TLS handshake : code=%d msg= %s\n", return_code, wolfSSL_get_error(ssl, return_code));
        print_error(err_msg);
    }
    print_log("TLS handshake successfull");
    // TLS handshake successful, echo received data back to the client
    char serverBuf[128];
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

socket_cleanup:
    if (sock)
        sock_tcp_disconnect(sock);
    if (ctx)
        wolfSSL_CTX_free(ctx);
    if (ssl)
        wolfssl_free(ssl);
    wolfSSL_Cleanup();
cleanup:

    sock_tcp_stop_listen(&queue);

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
