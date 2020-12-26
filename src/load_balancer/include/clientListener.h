#ifndef __CLIENT_LISTENER_H__
#define __CLIENT_LISTENER_H__

#include "secretary.h"

#define CLIENT_PORT                  7891
#define MAX_NO_OF_SECRETARIES        200

typedef struct CLIENT_LISTENER {
    int                     socket;
    struct sockaddr_in      server;

    pthread_t               thread_id;
} client_listener_t;

void init_client_listener();
void register_client(connections_t *connections,
                     int client_socket,
                     struct sockaddr_in *client_addr);

#endif 
