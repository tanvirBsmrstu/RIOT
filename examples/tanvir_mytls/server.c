

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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "wolfssl/ssl.h"
#include "wolfssl/certs_test.h"

// #include <wolfssl/options.h>

#define TLS_PORT 1222
#define CERT_FILE "../certs/server-cert.pem"

#define SOCK_QUEUE_LEN  (1U)
 
// sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
// uint8_t buf[128];


int tls_server(void)
{
    int                sockfd;
    char               buff[256];
    int                shutdown = 0;
    /* declare wolfSSL objects */
    WOLFSSL_CTX* ctx;
    WOLFSSL*     ssl;

     struct sockaddr_in6 servAddr;
    struct sockaddr_in6 clientAddr;
    socklen_t          size = sizeof(clientAddr);
    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR: failed to create the socket %d\n",sockfd);
        return -1;
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin6_family       = AF_INET6;             /* using IPv6      */
    servAddr.sin6_port         = htons(TLS_PORT);  /* on DEFAULT_PORT */

    
    /* Bind the server socket to local port */
    if (bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        fprintf(stderr, "ERROR: failed to bind\n");
        return -1;
    }

    /* Listen for a new connection, allow 5 pending connections */
    if (listen(sockfd, 3) == -1) {
        fprintf(stderr, "ERROR: failed to listen\n");
        return -1;
    }

    puts("This is the TLS Server!");
    printf("Server is listening on port %d\n",TLS_PORT);

    wolfSSL_Init();
    
    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        return -1;
    }
    
    //wolfSSL_CTX_set_verify(ctx,WOLFSSL_VERIFY_NONE);
    /* Load server certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_certificate_buffer(ctx, server_cert_der_2048,
                                         sizeof_server_cert_der_2048,
                                         SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load server_cert_der_2048\n");
        return -1;
    }

    /* Load server key into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, server_key_der_2048,
                                        sizeof_server_key_der_2048,
                                        SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load server_key_der_2048\n");
        return -1;
    }
printf("Server wolfssl init done\n");
    /* Continue to accept clients until shutdown is issued */
    while (!shutdown) {
        int  connd;
        size_t len;
        printf("Waiting for a connection...\n");

        /* Accept client connections */
        connd = accept(sockfd, (struct sockaddr *)&clientAddr, &size);
        if (connd < 0)  {
            fprintf(stderr, "ERROR: failed to accept the connection\n\n");
            continue;
        }
        printf("Server con accepted with %d\n",connd);
        /* Create a WOLFSSL object */
        if ((ssl = wolfSSL_new(ctx)) == NULL) {
            fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
            return -1;
        }

        printf("Attach wolfSSL to the socket\n");  
        wolfSSL_set_fd(ssl, connd);
        
        printf("Client connected successfully\n");

        /* Read the client data into our buff array */
        memset(buff, 0, sizeof(buff));
        if (wolfSSL_read(ssl, buff, sizeof(buff)-1) == -1) {
            fprintf(stderr, "ERROR: failed to read, state: %d\n", wolfSSL_state(ssl));
            return -1;
        }

        /* Print to stdout any data the client sends */
        printf("Client sent a message!\n");
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
        if (wolfSSL_write(ssl, buff, len) != (int) len) {
            fprintf(stderr, "ERROR: failed to write\n");
            return -1;
        }

        /* Cleanup after this connection */
        close(connd);           /* Close the connection to the client   */
    }

    printf("Shutdown complete\n");





////////////////////////////
    
    close(sockfd);  
    ////////////////// clean up /////////////
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

    return 0;
}

