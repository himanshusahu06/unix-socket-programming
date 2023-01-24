#include "common.h"
#include <time.h>

#define MAX 80
#define DEFAULT_PORT 8081

void handle_request(int sockfd)
{
    time_t curtime;
    int connfd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    char buff[MAX];
    while (1)
    {
        // Accept the data packet from client and verification.
        connfd = accept(sockfd, (SA *)&client, &len);
        if (connfd < 0)
        {
            err_quit("server accept failed...");
        }
        printf("server accept the client...\n");
        bzero(buff, sizeof(buff));
        // Read the message from client in buffer.
        // Read must be in while loop till read returns non zero values.
        read(connfd, buff, sizeof(buff));
        printf("From Client: %s\n", buff);
        // Write a response to buffer.
        time_t ticks = time(NULL);
        snprintf(buff, sizeof(buff), "Hello world from server - %.24s\r\n", ctime(&ticks));
        printf("To Client: %s\n", buff);
        write(connfd, buff, sizeof(buff));
        close(connfd);
    }
}

/**
 * This is iterative server because it iterates through each client, one at a time.
*/
int main(int argc, char **argv)
{
    int port = DEFAULT_PORT;
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    int listenfd;
    struct sockaddr_in serveraddr; // Server socket address
    // create a internet (AF_INET) stream (SOCK_STREAM) socket (TCP socket)
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket created successfully\n");
    bzero(&serveraddr, sizeof(serveraddr)); // zeroed bytes to the string
    serveraddr.sin_family = AF_INET;
    /**
     * htons - host to Network short.
     * It takes a 16-bit number in host byte order and returns a 16-bit number in network byte order used in TCP/IP networks.
     * Little endian, Bug endian concept
     */
    serveraddr.sin_port = htons(port);
    /**
     * htonl - host to network long
     * When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces.
     */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr));

    /**
     * Creation of socket-based connections requires several operations.
     * First, a socket is created with socket(2).
     * Next, a willingness to accept incoming connections and a queue limit for incoming connections are specified with listen().
     * Finally, the connections are accepted with accept(2).
     * The listen() call applies only to sockets of type SOCK_STREAM.
     * 
     * By calling listen, the socket is converted into a listening socket,
     * on which incoming connections from clients will be accepted by the kernel.
     * 
     * LISTENQEUE specifies the maximum number of client connections that the kernel
     * will queue for this listening descriptor.
     */
    if ((listen(listenfd, LISTENQEUE)) != 0)
    {
        err_sys("Listen failed...\n");
    }
    printf("Server listening...\n");

    // await connection from client, return response
    handle_request(listenfd);

    /**
     * close the socket.
     * Unix always closes all open descriptors when a process terminates,
     * so our TCP socket is now closed.
     */
    close(listenfd);
    printf("Server Exit...\n");
    return 0;
}
