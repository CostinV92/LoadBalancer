#ifndef __CLIENT_LISTENER_H__
#define __CLIENT_LISTENER_H__

#include "liblist.h"

#define CLIENT_PORT                  7891
#define MAX_NO_OF_SECRETARIES        200

typedef struct client client_t;

typedef struct client_listener {
    int                     socket;
    struct sockaddr_in      server;
    int                     max_socket;
    list_t                  *client_list;
} client_listener_t;

void client_listener_init();
void client_listener_destroy();
client_t *client_listener_new_client(int client_socket,
                                     struct sockaddr_in *client_addr);

void client_listener_check_client_sockets(int *num_socks, fd_set *read_sockets);
int client_listener_send_build_res(client_t* client, int status, int reason);

void client_listener_add_client_to_list(list_t *list, client_t *client);
void client_listener_delete_client_from_list(list_t *list, client_t *client);
void client_listener_add_worker_to_client(client_t *client, void *worker);
void client_listener_announce_clients(list_t* list);
const char *client_listener_get_ip_addr(client_t *client);
client_t *client_listener_get_client_from_address(list_t *list,
                                                  struct sockaddr_in *client_addr);
int client_listener_get_client_addr(client_t *client, struct sockaddr_in *client_address);
int client_listener_get_max_socket();

#ifdef DEBUG_CLIENT_WAIT_TIME
void client_listener_close_time_file();
#endif /* DEBUG_CLIENT_WAIT_TIME */

#endif 
