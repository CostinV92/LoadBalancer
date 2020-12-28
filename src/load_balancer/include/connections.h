#ifndef __SECRETARY_H__
#define __SECRETARY_H__

#include "utils.h"
#include "client_listener.h"
#include "worker_listener.h"
#include "liblist.h"

#define MAX_CLIENTS     10
#define MAX_WORKERS     10

typedef struct connections {
    fd_set              sockets;
    int                 max_socket;

    client_listener_t   *client_listener;
    worker_listener_t   *worker_listener;
} connections_t;

void start_listening();
void connections_unregister_socket(int client_socket);
void connections_process_message(void* peer, header_t* message, char* ip_addr);

int send_message(int socket, message_type_t type, int size, char* buffer);

#endif
