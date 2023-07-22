#include "common.h"
#include <event.h> // libevent
#include <fcntl.h>
#define DEFAULT_PORT 8081
#define TRUE 1
#define FALSE 0

struct event_base *base;

struct conn
{
    int fd;
    struct event ev_read;
};

void on_read_callback(int fd, short events, void *arg)
{
    struct conn *c = (struct conn *)arg;
    char buf[8196];
    int len;
    len = read(fd, buf, sizeof(buf));
    if (len == 0)
    {
        printf("Client disconnected\n");
        event_del(&c->ev_read);
        close(fd);
        free(c);
        return;
    }
    // Just assume we still have 5 chars left.
    buf[len - 1] = '-';
    buf[len++] = 'R';
    buf[len++] = 'E';
    buf[len++] = 'S';
    buf[len++] = 'P';
    buf[len++] = 'O';
    buf[len++] = 'N';
    buf[len++] = 'S';
    buf[len++] = 'E';
    buf[len++] = '\n';

    write(fd, buf, len); // just write the same data back.
}

void on_accept_callback(int fd, short events, void *arg)
{
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct conn *c;

    /* Accept the new connection. */
    client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        printf("Failed to connect to client\n");
        return;
    }

    /** Now, handle the connection by adding it to libevent for read */
    c = malloc(sizeof(struct conn *));
    c->fd = client_fd;
    event_set(&c->ev_read, client_fd, EV_READ | EV_PERSIST, on_read_callback, c);
    event_base_set(base, &c->ev_read);
    if (event_add(&c->ev_read, NULL) < 0)
        err_sys("Failed to add client FD in libevent");

    printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
}

void signal_callback(evutil_socket_t signal, short events, void *arg)
{
    if (signal == SIGUSR1)
        printf("Received signal: SIGUSR1\n");
    if (signal == SIGINT)
        printf("Received signal: SIGINT\n");
    struct event_base *base = arg;
    struct timeval delay = {2, 0};
    printf("shutting down in two seconds.\n");
    event_base_loopexit(base, &delay);
}

int main(int argc, char **argv)
{
    // 1. Setup server
    int port = DEFAULT_PORT;
    int reuseaddr_on = 1;
    if (argc == 2)
        port = atoi(argv[1]);
    struct event ev_accept;
    int server_fd;
    struct sockaddr_in serveraddr;
    server_fd = Socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on)) == -1)
    {
        err_sys("setsockopt failed");
    }
    printf("Server socket - [%d] created successfully..\n", server_fd);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(server_fd, (SA *)&serveraddr, sizeof(serveraddr));

    if ((listen(server_fd, LISTENQEUE)) != 0)
        err_sys("Listen failed...\n");

    // 2. Setup event base & add server FD in libevent
    base = event_base_new_with_config(NULL);
    event_set(&ev_accept, server_fd, EV_READ | EV_PERSIST, on_accept_callback, NULL);
    event_base_set(base, &ev_accept);
    if (event_add(&ev_accept, NULL) < 0)
        err_sys("Failed to add server socket in libevent");

    // 3.1 Add some event handlings
    if (event_add(evsignal_new(base, SIGINT, signal_callback, (void *)base), NULL) < 0)
        err_sys("Failed to add SIGINT handling.");
    if (event_add(evsignal_new(base, SIGUSR1, signal_callback, (void *)base), NULL) < 0)
        err_sys("Failed to add SIGUSR1 handling."); // kill -SIGUSR1 <pid>

    // Start event base loop
    event_base_dispatch(base);

    // Clean it up
    close(server_fd);
    event_base_free(base);

    return 0;
}
