#ifndef __SECRETARY_H__
#define __SECRETARY_H__

#include "utils.h"
#include "clientListener.h"
#include "workerListener.h"
#include "liblist.h"

#define MAX_CLIENTS     10
#define MAX_WORKERS     10

typedef struct connections {
    fd_set              sockets;
    int                 max_socket;

    client_listener_t   *client_listener;
    worker_listener_t   *worker_listener;
} connections_t;

void* assign_secretary(void*);
void start_listening();

#endif
