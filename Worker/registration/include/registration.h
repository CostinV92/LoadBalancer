#ifndef __REGISTRATION_H__
#define __REGISTRATION_H__

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define SERVER_PORT 7892

typedef struct LOAD_BALANCER {
    int                     socket;
    struct sockaddr_in      server;
} load_balancer_server_t;

load_balancer_server_t *loadBalancer;

void register_worker();

#endif