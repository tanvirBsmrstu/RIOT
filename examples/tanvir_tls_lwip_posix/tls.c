

#include <stdio.h>
#include <wolfssl/wolfcrypt/settings.h>
#include "wolfssl/ssl.h"
#include "wolfssl/certs_test.h"
#include <stdbool.h>
// #include "net/sock/tcp.h"
// #include <wolfssl/wolfcrypt/settings.h>
// #include <wolfssl/ssl.h>
// #include "wolfssl/certs_test.h"
// #include <arpa/inet.h>
// #include <netinet/in.h>

// #include "net/af.h"

/* socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "log.h"

// #include <wolfssl/options.h>

extern const unsigned char server_cert[];
extern const unsigned char server_key[];
extern unsigned int server_cert_len;
extern unsigned int server_key_len;

//////////////////////////////////////////////////

WOLFSSL_CTX *init_wolfSSL_ctx_util(bool isServer)
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

    return ctx;
    // Create a wolfSSL object for the connection
    // if ((ssl = wolfSSL_new(ctx)) == NULL)
    // {
    //     printf("SSL object creation failed\n");
    // }
    // else
    // {
    //     printf("SSL object has been create successfully\n");
    // }
    // printf("SSL object setting I//O...\n");
    // // Set up the file descriptor for WolfSSL object (client socket)
    // // wolfSSL_SetIORecv(ssl, sock_tcp_recv);
    // // wolfSSL_SetIOSend(ssl, sock_tcp_send);
    // // wolfSSL_SetIOCTX(ssl, &client_socket);

    // return ssl;
}

WOLFSSL_CTX *init_wolfSSL_server(void)
{
    WOLFSSL_CTX *ctx = init_wolfSSL_ctx_util(true);
    return ctx;
}
WOLFSSL_CTX *init_wolfSSL_client(void)
{
    WOLFSSL_CTX *ctx = init_wolfSSL_ctx_util(false);
    /* Disable certificate validation from the client side */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
    return ctx;
}
////////////////////////////////////////////////////

#define TLS_PORT 1222

int tls_server(void)
{
    puts("This is the TLS Server!");

    int server_socket = SOCKET_INVALID, client_socket = SOCKET_INVALID;
    struct sockaddr_in6 servAddr;
    struct sockaddr_in6 clientAddr;
    socklen_t size = sizeof(clientAddr);
    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket %d\n", server_socket);
        goto end;
    }
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin6_family = AF_INET6;      /* using IPv6      */
    servAddr.sin6_port = htons(TLS_PORT); /* on DEFAULT_PORT */

    /* Bind the server socket to local port */
    if (bind(server_socket, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        fprintf(stderr, "ERROR: failed to bind\n");
        goto clean_socket;
    }

    /* Listen for a new connection, allow 5 pending connections */
    if (listen(server_socket, 3) == -1)
    {
        fprintf(stderr, "ERROR: failed to listen\n");
        goto clean_socket;
    }
    printf("Server is listening on port %d\n", TLS_PORT);

    if ((client_socket = accept(server_socket, (struct sockaddr *)&clientAddr, &size)) < 0)
    {
        fprintf(stderr, "ERROR: failed to accept the connection\n\n");
        goto clean_socket;
    }
    printf("Server TCP connection done\n");
    WOLFSSL_CTX *ctx = init_wolfSSL_server();
    WOLFSSL *ssl = NULL;

    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        printf("SSL object creation failed\n");
        goto clean_ctx;
    }
    else
    {
        printf("SSL object has been create successfully\n");
    }
    wolfSSL_set_fd(ssl, client_socket);

    printf("Server wolfssl init done, waiting for tls client\n");

    /* Establish TLS connection */
    int ret = wolfSSL_accept(ssl);
    if (ret != WOLFSSL_SUCCESS)
    {
        fprintf(stderr, "wolfSSL_accept error = %d\n",
                wolfSSL_get_error(ssl, ret));
        goto clean_ssl;
    }
    printf("Server TLS client connected\n");

    // /* Read the client data into our buff array */
    // memset(buff, 0, sizeof(buff));
    // if (wolfSSL_read(ssl, buff, sizeof(buff) - 1) == -1)
    // {
    //     fprintf(stderr, "ERROR: failed to read, state: %d\n", wolfSSL_state(ssl));
    //     return -1;
    // }

    // /* Print to stdout any data the client sends */
    // printf("Client sent a message!\n");
    // printf("Client said: %s\n", buff);

    // /* Check for server shutdown command */
    // if (strncmp(buff, "shutdown", 8) == 0)
    // {
    //     printf("Shutdown command issued!\n");
    //     shutdown = 1;
    // }

    // /* Write our reply into buff */
    // strncpy(buff, "I hear ya fa shizzle!\n", sizeof(buff) - 1);
    // len = strnlen(buff, sizeof(buff));
    // printf("Sending reply to client, reply reads: %s\n", buff);

    // /* Reply back to the client */
    // if (wolfSSL_write(ssl, buff, len) != (int)len)
    // {
    //     fprintf(stderr, "ERROR: failed to write\n");
    //     return -1;
    // }

    printf("Shutdown complete\n");

    ////////////////// clean up ///////////////////
clean_ssl:
    wolfSSL_free(ssl);

clean_ctx:
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

clean_socket:
    if (client_socket != SOCKET_INVALID)
        close(client_socket);

    if (server_socket != SOCKET_INVALID)
        close(server_socket);
end:
    return 0;
}

////////////////////////////////////////////// client .////////////////////////

int tls_client(char *server_ip)
{

    int client_socket = 0;
    puts("This is a TLS Client!");

    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((client_socket = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        goto end;
    }
    struct sockaddr_in6 servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin6_family = AF_INET6;      /* using IPv6      */
    servAddr.sin6_port = htons(TLS_PORT); /* on SERVER_PORT */

    /* Get the server IPv6 address from the compile-time string parameter */
    if (inet_pton(AF_INET6, server_ip, &servAddr.sin6_addr.s6_addr) != 1)
    {
        fprintf(stderr, "ERROR: invalid address\n");
        goto clean_socket;
    }

    /* Connect to the server */
    if (connect(client_socket, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        fprintf(stderr, "ERROR: failed to connect\n");
        goto clean_socket;
    }
    printf("TCP connection done, now trying tls\n");

    //////////  TCP connection done //////////////

    WOLFSSL_CTX *ctx = init_wolfSSL_client();
    WOLFSSL *ssl = NULL;
    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
        goto clean_ctx;
    }
    int ret = wolfSSL_set_fd(ssl, client_socket);

    //////////////// wolfssl setup done /////////////

    printf("Attach response wolfSSL to the socket...%d ... %d\n", ret, wolfSSL_get_error(ssl, ret));
    /* TLS handshake */
    if ((ret = wolfSSL_connect(ssl)) != SSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: failed to connect to wolfSSL: %d  error %d\n", ret, wolfSSL_get_error(ssl, ret));
        goto clean_ssl;
    }
    // wolfSSL_CTX_SetIORecv(ctx, data_recv);
    // wolfSSL_CTX_SetIOSend(ctx, data_send);
    //   printf("starting read write..");
    // /* Send the message to the server */
    // char buff[] = "Hello, TLS Server!\0";
    // int len = strlen(buff);
    // if (wolfSSL_write(ssl, buff, len) != len)
    // {
    //     fprintf(stderr, "ERROR: failed to write\n");
    //     return -1;
    // }
    // printf("Data send : %s\n", buff);
    // /* Read the server data into our buff array */
    // memset(buff, 0, sizeof(buff));
    // if (wolfSSL_read(ssl, buff, sizeof(buff) - 1) == -1)
    // {
    //     fprintf(stderr, "ERROR: failed to read\n");
    //     return -1;
    // }
    // printf("Data arrives : %s\n", buff);

    ////////////////// clean up ///////////////////
clean_ssl:
    wolfSSL_free(ssl);

clean_ctx:
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

clean_socket:
    close(client_socket);
end:
    return 0;
}
