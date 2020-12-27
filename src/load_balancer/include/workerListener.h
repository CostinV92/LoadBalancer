#ifndef __WORKER_LISTENER_H__
#define __WORKER_LISTENER_H__

#include "heap.h"
#include "liblist.h"

#define WORKER_PORT         7892

typedef struct WORKER_LISTENER {
    int                     socket;
    struct sockaddr_in      server;
    list_t                  *worker_list;

    pthread_t               thread_id;
} worker_listener_t;


typedef struct WORKER {
    heap_node_t             heap_node;

    // network info
    int                     socket;
    struct sockaddr_in      addr;
    list_node_t             list_node;

    // worker info
    char                    hostname[256];
    int                     no_current_builds;
    pthread_mutex_t         mutex;
    bool                    alive;
} worker_t;

void init_worker_listener();
void worker_listener_new_worker(int worker_socket,
                                struct sockaddr_in *worker_addr);

#endif
