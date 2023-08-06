
#ifndef WOLFSSL_COMMON_TLS_H
    #define WOLFSSL_COMMON_TLS_H

    #include <wolfssl/wolfcrypt/settings.h>
    #include <wolfssl/ssl.h>
    #include "wolfssl/certs_test.h"

    WOLFSSL_CTX *init_wolfSSL_server(CallbackIOSend io_send, CallbackIORecv io_recv);
    WOLFSSL_CTX *init_wolfSSL_client(CallbackIOSend io_send, CallbackIORecv io_recv);

#endif
