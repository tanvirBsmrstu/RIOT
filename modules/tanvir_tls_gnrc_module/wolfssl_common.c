
#include "wolfssl_common.h"
#include "log.h"

// typedef int (*FuncSignature)(int, int);

WOLFSSL_CTX *init_wolfSSL_server(CallbackIOSend io_send, CallbackIORecv io_recv)
{
    WOLFSSL_CTX *ctx = NULL;
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

    /* Set custom I/O callbacks and context */
    wolfSSL_CTX_SetIORecv(ctx, io_recv);
    wolfSSL_CTX_SetIOSend(ctx, io_send);

    return ctx;
}
WOLFSSL_CTX *init_wolfSSL_client(CallbackIOSend io_send, CallbackIORecv io_recv)
{
    WOLFSSL_CTX *ctx = NULL;
    wolfSSL_Init();

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        return NULL;
    }

    /* Load client certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
                                       sizeof_ca_cert_der_2048,
                                       SSL_FILETYPE_ASN1) != SSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: failed to load ca buffer\n");
        return NULL;
    }

    /* Set custom I/O callbacks and context */
    wolfSSL_CTX_SetIORecv(ctx, io_recv);
    wolfSSL_CTX_SetIOSend(ctx, io_send);

    /* Disable certificate validation from the client side */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);

    return ctx;
}
