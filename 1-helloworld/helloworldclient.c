#include "common.h"

#define MAX 80

void send_request(int sockfd)
{
    char buff[MAX];
    bzero(buff, MAX);
    strcpy(buff, "Hello from client");
    printf("To Server: %s\n", buff);
    write(sockfd, buff, sizeof(buff));
    bzero(buff, MAX);
    // read must be in while loop till read returns non zero values.
    read(sockfd, buff, sizeof(buff));
    printf("From Server: %s\n", buff);
    printf("Done\n");
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in serveraddr;

    if (argc != 2)
    {
        err_quit("usage: ./client.o <server port>");
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    Connect(sockfd, (SA *)&serveraddr, sizeof(serveraddr));

    printf("connected to server..\n");

    send_request(sockfd);
    close(sockfd);
    return 0;
}