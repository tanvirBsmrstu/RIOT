
#include "tls_api_types.h"

#include <net/sock/tcp.h>

struct tls_api_socket_t{
    sock_tcp_t* socket;
}tls_api_socket_t;
