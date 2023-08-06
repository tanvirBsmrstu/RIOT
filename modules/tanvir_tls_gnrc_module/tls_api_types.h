
#ifndef TLS_API_TYPES_H
#define TLS_API_TYPES_H

typedef void *app_ctx_t;
#include <net/sock/tcp.h>

struct tls_api_socket_t
{
    sock_tcp_t *socket;
};
typedef struct tls_api_socket_t tls_api_socket_t;

#endif
