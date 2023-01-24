#include "common.h"

int Socket(int family, int protocol, int type)
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        err_sys("socket error");
    }
    return sockfd;
}

void Bind(int socketfd, const struct sockaddr *serveraddr, int socklen)
{
    if (bind(socketfd, serveraddr, socklen) != 0)
    {
        err_sys("Binding failed!\n");
    }
}

void Connect(int socketfd, const struct sockaddr *serveraddr, int socketlen) {
    if (connect(socketfd, serveraddr, socketlen) < 0)
    {
        err_sys("connect error");
    }
}
