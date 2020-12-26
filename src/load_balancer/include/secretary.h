#ifndef __SECRETARY_H__
#define __SECRETARY_H__

#include "utils.h"

typedef struct CLIENT {
    int                     socket;
    struct sockaddr_in      addr;

    char                    hostname[256];
} client_t;

#define MAX_CLIENTS     10
#define MAX_WORKERS     10

typedef struct connections {
    int                 num_sockets;
    fd_set              sockets;
    int                 max_socket;

    int                 num_client_sockets;
    int                 client_sockets[MAX_CLIENTS];

    int                 num_worker_sockets;
    int                 worker_sockets[MAX_WORKERS];
} connections_t;

void* assign_secretary(void*);
void start_listening();

#endif
