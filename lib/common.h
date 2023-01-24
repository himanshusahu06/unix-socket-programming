#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h> // read(), write(), close()

#define SA struct sockaddr
#define LISTENQEUE 1024

// import all libs here.
#include "error.h"
#include "sock.h"