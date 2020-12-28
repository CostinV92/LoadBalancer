#ifndef __CLIENT_LISTENER_H__
#define __CLIENT_LISTENER_H__

#include "liblist.h"

#define CLIENT_PORT                  7891
#define MAX_NO_OF_SECRETARIES        200

typedef struct client client_t;

typedef struct CLIENT_LISTENER {
    int                     socket;
    struct sockaddr_in      server;
    list_t                  *client_list;

    pthread_t               thread_id;
} client_listener_t;

void init_client_listener();
void client_listener_new_client(int client_socket,
                                struct sockaddr_in *client_addr);
list_t* client_listener_get_client_list();
void client_listener_message_from_client(client_t *client);
void client_listener_check_client_sockets(int *num_socks, fd_set *read_sockets);
struct sockaddr_in client_listener_get_client_addr(client_t *client);

#endif 
