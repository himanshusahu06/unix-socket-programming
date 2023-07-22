#include "common.h"
#include <time.h>

#define MAX 80
#define DEFAULT_PORT 8081

void handle_request(int sockfd)
{
    int client_fd;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    char read_buffer[MAX];
    char write_buffer[MAX];
    // Process each connection one by one.
    while (1)
    {
        // STEP 1: Accept client connection.
        client_fd = accept(sockfd, (SA *)&client, &len);
        if (client_fd < 0)
        {
            err_quit("Failed to accept client connection. Exiting.");
        }
        bzero(read_buffer, sizeof(read_buffer));

        // ----------------- Just print client info ------------------------
        char client_ip[INET_ADDRSTRLEN];
        int client_port = (int)ntohs(client.sin_port);
        inet_ntop(AF_INET, &client.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Client [%s:%d] connected\n", client_ip, client_port);
        // -----------------------------------------------------------------

        // OOPS! server is waiting here.
        int bytes_read = read(client_fd, read_buffer, sizeof(read_buffer));
        printf("Client [%s:%d] wrote: %s", client_ip, client_port, read_buffer);

        time_t ticks = time(NULL);
        // Write a response to client.
        snprintf(write_buffer, sizeof(write_buffer),
                 "Hello from server - %.24s\r\n", ctime(&ticks));
        printf("Client [%s:%d] Server replied: %s",
               client_ip, client_port, write_buffer);
        write(client_fd, write_buffer, sizeof(write_buffer));

        // Close the connection
        close(client_fd);
        printf("Client [%s:%d] disconnected\n\n", client_ip, client_port);
    }
}

int main(int argc, char **argv)
{
    // STEP 1: Create a socket, bind it with port and listen for incoming connection.
    int server_fd;
    struct sockaddr_in serveraddr;
    server_fd = Socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket created successfully\n");
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(DEFAULT_PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        err_sys("setsockopt(SO_REUSEADDR) failed");
    Bind(server_fd, (SA *)&serveraddr, sizeof(serveraddr));
    if ((listen(server_fd, LISTENQEUE)) != 0)
        err_sys("Listen failed...\n");
    printf("Server listening on port %d\n\n", DEFAULT_PORT); // woo.hoo..

    // STEP 2: await client connection and server the request.
    handle_request(server_fd);

    // STEP 3: Close the server gracefully but let's skip it for now.
    return 0;
}
