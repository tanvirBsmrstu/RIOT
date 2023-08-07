

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
#include <sys/select.h>
#include "net/ipv6/addr.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>


// #include <wolfssl/options.h>

#define TCP_POSIX_PORT 1232
#define CERT_FILE "../certs/server-cert.pem"

#define SOCK_QUEUE_LEN (5U)

// sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
uint8_t buf[128];

int tcp_posix_server(void)
{
    int server_socket=0, client_socket=0; // listening socket, connection socket
    char buff[256];

    /* declare wolfSSL objects */
    printf("This is a posix tcp server with gnrc netstack");
    struct sockaddr_in6 servAddr;

    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        return -1;
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));
    /* Fill in the server address */
    servAddr.sin6_family = AF_INET6;      /* using IPv6      */
    servAddr.sin6_port = htons(TCP_POSIX_PORT); /* on DEFAULT_PORT */

    /* Bind the server socket to local port */
    if (bind(server_socket, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        fprintf(stderr, "ERROR: failed to bind\n");
        goto sock_close;
    }

    /* Listen for a new connection, allow 5 pending connections */
    if (listen(server_socket, 5) == -1)
    {
        fprintf(stderr, "ERROR: failed to listen\n");
        goto sock_close;
    }

    printf("Server is listening on port %d\n", TCP_POSIX_PORT);

    struct sockaddr_in6 clientAddr;
    socklen_t size = sizeof(clientAddr);
    /* Continue to accept clients until shutdown is issued */

    size_t len;
    printf("Waiting for a connection...\n");

    /* Accept client connections */

    if ((client_socket = accept(server_socket, (struct sockaddr *)&clientAddr, &size)) < 0)
    {
        fprintf(stderr, "ERROR: failed to accept the connection\n\n");
        goto sock_close;
    }
    printf("Client connected successfully\n");
      printf("Waiting for Reading...\n");
    int valread = read(client_socket, buff, strlen(buff));
    if (valread > 0)
    {
        // Handle data received from the client
        printf("Received from client: %s\n", buff);

        // Clear the buffer for the next message
        memset(buff, 0, sizeof(buff));
    }
    else if (valread == 0)
    {
        puts("Client disconnected");
         goto sock_close;
    }
    printf("Writing to client...\n");
    // Check if there's data to be sent to the client
    // For simplicity, we'll just use a fixed message to send
    char *response = "Server received your message.";
    send(client_socket, response, strlen(response), 0);

sock_close:
    /* Cleanup after this connection */
    close(client_socket); /* Close the connection to the client   */
    close(server_socket);
    printf("Shutdown complete\n");
    return 0;
}



int tcp_posix_client(char *server_ip)
{
    int BUFFER_SIZE = 20;
    char buffer[BUFFER_SIZE];
    int client_socket = 0;
    
    /* Create a socket that uses an internet IPv6 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((client_socket = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        goto sock_close;
    }
    struct sockaddr_in6 servAddr;
    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));
    /* Fill in the server address */
    servAddr.sin6_family = AF_INET6;      /* using IPv6      */
    servAddr.sin6_port = TCP_POSIX_PORT; /* on SERVER_PORT */

    /* Get the server IPv6 address from the compile-time string parameter */
    if (inet_pton(AF_INET6, server_ip, &servAddr.sin6_addr.s6_addr) != 1)
    {
        fprintf(stderr, "ERROR: invalid address\n");
        goto sock_close;
    }
    printf("posix client : Trying to connect to %s port: %d\n",server_ip,TCP_POSIX_PORT);
    /* Connect to the server */
    if (connect(client_socket, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        fprintf(stderr, "ERROR: failed to connect [%s]\n",strerror(errno));
        goto sock_close;
    }
    printf("TCP connection done with client socket: %d\n", client_socket);
    ///////////////////////////////////////
    // sock_tcp_t c_socket;
    // sock_tcp_get_fd(&c_socket);
    printf("Sending msg to server...\n");
    char *message = "Hello from the client!";
    if (-1 == send(client_socket, message, strlen(message), 0))
    {
        printf("sending msg to server failed\n");
    }
    else
    {
        printf("sending msg to server done\n");
    }

     printf("Waiting for Reading...\n");
    // Receive the response from the server
    int valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread > 0)
    {
        printf("Received from server: %s\n", buffer);
    }
sock_close:
    // Close the client socket
    close(client_socket);

    return 0;
}
