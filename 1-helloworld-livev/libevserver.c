#include "common.h"
#include <time.h>
#include <event.h> // libevent
#include <fcntl.h>
#define MAX 80
#define DEFAULT_PORT 8081
#define TRUE 1
#define FALSE 0

struct event_base *base;

struct conn
{
    int fd;
    struct event ev_read;
    struct sockaddr_in client_addr;
};

void on_read_cb(int fd, short events, void *arg)
{
    struct conn *c = (struct conn *)arg;
    char rbuff[8196];
    bzero(rbuff, sizeof(rbuff));

    // read data from FD, valdiate if it's a closed connection.
    if (read(fd, rbuff, sizeof(rbuff)) == 0)
    {
        printf("Client [%s:%d] disconnected\n",
               inet_ntoa(c->client_addr.sin_addr), (int)ntohs(c->client_addr.sin_port));
        // Connection is closed, no need to watch again
        event_del(&c->ev_read);
        close(fd);
        free(c);
        return;
    }
    printf("Client [%s:%d] wrote: %s",
           inet_ntoa(c->client_addr.sin_addr), (int)ntohs(c->client_addr.sin_port), rbuff);

    // write back to client.
    char wbuff[MAX];
    bzero(wbuff, sizeof(wbuff));
    time_t ticks = time(NULL);
    snprintf(wbuff, sizeof(wbuff),
             "Hello from server - %.24s\r\n", ctime(&ticks));
    printf("Client [%s:%d] Server replied: %s",
           inet_ntoa(c->client_addr.sin_addr), (int)ntohs(c->client_addr.sin_port), wbuff);
    write(fd, wbuff, sizeof(wbuff));
}

// Once server socket accepts a connections, it creates a conn object and add
// client socket back to libevent with "on_read_cb" callback which essentially
// handles any data read from client.
void on_accept_cb(int fd, short events, void *arg)
{
    struct event_base *libevbase = (struct event_base *)arg;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct conn *c;

    /* Accept the new connection. accept() returns a client socket */
    client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        printf("Failed to connect to client\n");
        return;
    }

    /** Now, handle the connection by adding it to libevent for read */
    c = malloc(sizeof(struct conn *));
    c->fd = client_fd;
    c->client_addr = client_addr; // just add it for fun
    event_set(&c->ev_read, client_fd, EV_READ | EV_PERSIST, on_read_cb, c);
    event_base_set(libevbase, &c->ev_read);
    if (event_add(&c->ev_read, NULL) < 0)
        err_sys("Failed to add client FD to libevent.");

    printf("Client [%s:%d] connected\n",
           inet_ntoa(client_addr.sin_addr), (int)ntohs(client_addr.sin_port));
}

void signal_cb(evutil_socket_t signal, short events, void *arg)
{
    if (signal == SIGUSR1)
        printf("Received signal: SIGUSR1\n");
    if (signal == SIGINT)
        printf("Received signal: SIGINT\n");
    struct event_base *base = arg;
    struct timeval delay = {1, 0};
    printf("shutting down in one seconds.\n");
    event_base_loopexit(base, &delay);
}

int main(int argc, char **argv)
{
    // 1. Setup server -------------- 1)
    int port = DEFAULT_PORT;
    int reuseaddr_on = 1;
    if (argc == 2)
        port = atoi(argv[1]);
    int server_fd;
    struct sockaddr_in serveraddr;
    server_fd = Socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(server_fd,
                   SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, sizeof(reuseaddr_on)) == -1)
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
    struct event ev_accept;
    base = event_base_new_with_config(NULL);
    event_set(&ev_accept, server_fd, EV_READ | EV_PERSIST, on_accept_cb, base);
    event_base_set(base, &ev_accept);
    if (event_add(&ev_accept, NULL) < 0)
        err_sys("Failed to add server socket in libevent");

    // 3.1 Add some event handlings
    if (event_add(evsignal_new(base, SIGINT, signal_cb, (void *)base), NULL) < 0)
        err_sys("Failed to add SIGINT handling.");
    if (event_add(evsignal_new(base, SIGUSR1, signal_cb, (void *)base), NULL) < 0)
        err_sys("Failed to add SIGUSR1 handling."); // kill -SIGUSR1 <pid>

    // Start event base loop
    event_base_dispatch(base);

    // Clean it up
    close(server_fd);
    event_base_free(base);

    return 0;
}
