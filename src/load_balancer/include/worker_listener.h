#ifndef __WORKER_LISTENER_H__
#define __WORKER_LISTENER_H__

#include "heap.h"
#include "client_listener.h"
#include "liblist.h"
#include "messages.h"

#define WORKER_PORT         7892

typedef struct worker worker_t;

typedef struct WORKER_LISTENER {
    int                     socket;
    struct sockaddr_in      server;
    list_t                  *worker_list;

    pthread_t               thread_id;
} worker_listener_t;

void init_worker_listener();
void worker_listener_new_worker(int worker_socket,
                                struct sockaddr_in *worker_addr);
list_t* worker_listener_get_worker_list();
void worker_listener_check_worker_sockets(int *num_socks, fd_set *read_sockets);

/* TODO(victor): renamie this */
void process_build_req(client_t* client, build_req_msg_t* message);
bool send_build_order(worker_t* worker, client_t* client, build_req_msg_t* build_message);

client_t *worker_listener_get_client_from_address(worker_t *worker,
                                                  struct sockaddr_in *client_addr);
void worker_listener_decrement_no_of_builds_and_update_node_key(worker_t *worker);

#endif
