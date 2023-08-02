#include "net/af.h"
#include "net/ipv6/addr.h"
// the above two library is needed for the client//

#include "net/sock/tcp.h"


#define TcpPort 12343


#define SOCK_QUEUE_LEN  (1U)
 
sock_tcp_t sock_queue[SOCK_QUEUE_LEN];

 
int tcpServer(void)
{
   
    int res;
    sock_tcp_queue_t queue;
    sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
  
 
    local.port = TcpPort;
 
    if ((res=sock_tcp_listen(&queue, &local, sock_queue, SOCK_QUEUE_LEN, 0)) < 0) {
        printf("Error creating listening queue %d\n",res);
        return 1;
    }
    printf("Listening on port %d\n",TcpPort);
    uint8_t buf[128];
    while (1) {
        sock_tcp_t *sock;
 
        if (sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT) < 0) {
            puts("Error accepting new sock");
        }
        else {
            int read_res = 0;
 
            puts("Reading data");
            while (read_res >= 0) {
                read_res = sock_tcp_read(sock, &buf, sizeof(buf),
                                         SOCK_NO_TIMEOUT);
                if (read_res <= 0) {
                    puts("Disconnected");
                    break;
                }
                else {
                    int write_res;
                    printf("Read: \"");
                    for (int i = 0; i < read_res; i++) {
                        printf("%c", buf[i]);
                    }
                    puts("\"");
                    if ((write_res = sock_tcp_write(sock, &buf,
                                                    read_res)) < 0) {
                        puts("Errored on write, finished server loop");
                        break;
                    }
                }
            }
            sock_tcp_disconnect(sock);
        }
    }
    sock_tcp_stop_listen(&queue);
    return 0;
}
////////////////////////////////////////////// client ////////////////////////////

 
int tcpClient(char * remoteIP)
{
    int res;
    sock_tcp_t sock;
    sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;
 
    remote.port = TcpPort;
    uint8_t buf[128];

    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr,remoteIP);
    if (sock_tcp_connect(&sock, &remote, 0, 0) < 0) {
        puts("Error connecting sock");
        return 1;
    }
    puts("Sending \"Hello!\"");
    if ((res = sock_tcp_write(&sock, "Hello!", sizeof("Hello!"))) < 0) {
        puts("Errored on write");
    }
    else {
        if ((res = sock_tcp_read(&sock, &buf, sizeof(buf),
                                 SOCK_NO_TIMEOUT)) <= 0) {
            puts("Disconnected");
        }
        printf("Read: \"");
        for (int i = 0; i < res; i++) {
            printf("%c", buf[i]);
        }
        puts("\"");
    }
    sock_tcp_disconnect(&sock);
    return res;
}
