

#include <stdio.h>
#include "net/sock/tcp.h"
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include "wolfssl/certs_test.h"
#include <arpa/inet.h>
#include <netinet/in.h>

#include "net/af.h"

// #include <wolfssl/options.h>

#define TLS_PORT 11111
#define CERT_FILE "../certs/server-cert.pem"

int data_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    printf("Data arrives : %s\n", buf);
    return 0;
}
int data_send(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    printf("Data send : %s\n", buf);
    return 0;
}

int tls_client(int argc, char **argv)
{

    int sockfd = 0;

    char *server_ip = NULL;

    puts("This is a TLS Client!");
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s IP_ADDR_SERVER\n", argv[0]);
        return -1;
    }
    printf("Client is connecting to server at address %s port %d...\n", argv[1], TLS_PORT);
    server_ip = argv[1];

    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        return -1;
    }
    struct sockaddr_in6 servAddr;
    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));
    /* Fill in the server address */
    servAddr.sin6_family = AF_INET6;      /* using IPv6      */
    servAddr.sin6_port = htons(TLS_PORT); /* on SERVER_PORT */

    /* Get the server IPv6 address from the compile-time string parameter */
    if (inet_pton(AF_INET6, server_ip, &servAddr.sin6_addr.s6_addr) != 1)
    {
        fprintf(stderr, "ERROR: invalid address\n");
        return -1;
    }

    /* Connect to the server */
    if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        fprintf(stderr, "ERROR: failed to connect\n");
        return -1;
    }
    printf("TCP connection done\n");
    //////////  TCP connection done //////////////

    wolfSSL_Init();
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        return -1;
    }
    // wolfSSL_CTX_set_cipher_list(ctx, "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256");
    /* Load client certificates into WOLFSSL_CTX */
    // if (wolfSSL_CTX_load_verify_locations(ctx, CERT_FILE, NULL)
    //     != SSL_SUCCESS) {
    //     fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
    //             CERT_FILE);
    //     return -1;
    // }

    /* Disable certificate validation from the client side */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
    printf("Now load certificates...\n");
    /* Load client certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
                                       sizeof_ca_cert_der_2048,
                                       SSL_FILETYPE_ASN1) != SSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: failed to load ca buffer\n");
        exit(-1);
    }
    printf("Now load ssl...\n");
    /* Create a WOLFSSL object */
    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
        return -1;
    }
    printf("Attach wolfSSL to the socket...%d\n", sockfd);
    /* Attach wolfSSL to the socket */
    int ret = wolfSSL_set_fd(ssl, sockfd);
    printf("Attach response wolfSSL to the socket...%d ... %d\n", ret, wolfSSL_get_error(ssl, ret));
    /* Connect to wolfSSL on the server side */

    /* Connect to wolfSSL on the server side */
    if ((ret=wolfSSL_connect(ssl)) != SSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: failed to connect to wolfSSL: %d\n", ret);
        exit(-1);
    }
    // wolfSSL_CTX_SetIORecv(ctx, data_recv);
    // wolfSSL_CTX_SetIOSend(ctx, data_send);
      printf("starting read write..");
    /* Send the message to the server */
    char buff[] = "Hello, TLS Server!\0";
    int len = strlen(buff);
    if (wolfSSL_write(ssl, buff, len) != len)
    {
        fprintf(stderr, "ERROR: failed to write\n");
        return -1;
    }
    printf("Data send : %s\n", buff);
    /* Read the server data into our buff array */
    memset(buff, 0, sizeof(buff));
    if (wolfSSL_read(ssl, buff, sizeof(buff) - 1) == -1)
    {
        fprintf(stderr, "ERROR: failed to read\n");
        return -1;
    }
    printf("Data arrives : %s\n", buff);

    ////////////////// clean up /////////////
    close(sockfd);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

    return 0;
}
