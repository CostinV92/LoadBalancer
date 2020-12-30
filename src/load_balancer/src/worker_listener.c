#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include "worker_listener.h"

#include "libheap.h"
#include "libutils.h"
#include "connections.h"

typedef struct worker {
    heap_node_t             heap_node;

    int                     socket;
    struct sockaddr_in      addr;
    char                    ip_addr[MAX_IP_ADDR_SIZE];
    list_node_t             list_node;

    int                     no_current_builds;

    list_t                  *client_list;
} worker_t;

worker_listener_t *worker_listener;
heap_t *worker_heap;

extern void clean_exit(int status);

static void worker_listener_create();
static void worker_listener_free_worker(worker_t *worker);
static void worker_listener_new_max_socket();
static list_t* worker_listener_get_worker_list();

void worker_listener_init()
{
    int socket = 0;

    worker_listener = calloc(1, sizeof(worker_listener_t));
    if (!worker_listener) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    worker_listener->worker_list = list_new();
    if (!worker_listener->worker_list) {
        LOG("Error: %s() cannot allocate worker list.", __FUNCTION__);
        clean_exit(-1);
    }

    worker_heap = heap_init();
    if (!worker_heap) {
        LOG("Error: %s() failed to init worker heap.", __FUNCTION__);
        clean_exit(-1);
    }

    worker_listener_create();
}

void worker_listener_destroy()
{
    worker_t *worker = NULL;
    list_it *it = NULL;

    if (!worker_listener || !worker_listener->worker_list)
        return;

    list_iterate(worker_listener->worker_list, it) {
        worker = list_info_from_it(it, list_node, worker_t);

        list_node_delete(worker_listener->worker_list, &worker->list_node);
        list_delete(&worker->client_list);

        worker_listener_free_worker(worker);
    }

    list_delete(&worker_listener->worker_list);
    heap_destroy(&worker_heap);
    close(worker_listener->socket);
    free(worker_listener);
}

static void worker_listener_create()
{
    int server_socket, port, iSetOption = 1;
    struct sockaddr_in server_addr;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        LOG("Error: %s() error on opening worker listener socket.", __FUNCTION__);
        clean_exit(-1);
    }
    setsockopt(server_socket,
               SOL_SOCKET,
               SO_REUSEADDR,
               (char*)&iSetOption,
               sizeof(iSetOption));

    // init address structure
    memset(&server_addr, 0, sizeof(server_addr));

    port = WORKER_PORT;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // bind the socket with the address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
        LOG("Error: %s() error on biding worker listener socket.", __FUNCTION__);
        clean_exit(-1);
    }

    // mark socket as pasive socket
    if (listen(server_socket, 20) < 0) {
        LOG("Error: %s() error on marking worker listener socket as passive.", __FUNCTION__);
        clean_exit(-1);
    }

    // save listener info
    worker_listener->socket = server_socket;
    worker_listener->server = server_addr;
}

worker_t *worker_listener_new_worker(int worker_socket,
                                     struct sockaddr_in *worker_addr)
{
    if (!worker_listener) {
        LOG("Error: %s() worker_listener not initialized.");
        return NULL;
    }

    worker_t* worker = calloc(1, sizeof(worker_t));
    if (!worker) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    worker->socket = worker_socket;
    worker->addr = *worker_addr;
    utils_format_ip_addr(worker_addr, worker->ip_addr);

    if (worker_socket > worker_listener->max_socket)
        worker_listener->max_socket = worker_socket;

    list_node_init(&worker->list_node);
    list_add_back(worker_listener->worker_list, &worker->list_node);

    worker->client_list = list_new();

    worker_listener_add_worker_to_heap(worker);

    LOG("Worker: new worker %s.", worker->ip_addr);

    return worker;
}

static void worker_listener_free_worker(worker_t *worker)
{
    list_t *list = NULL;

    if (!worker) {
        LOG("error: %s invalid parameter.", __FUNCTION__);
        return;
    }

    list = worker_listener_get_worker_list();
    if (!list) {
        LOG("error: %s worker list not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    if (worker->socket == worker_listener->max_socket)
        worker_listener_new_max_socket();

    connections_unregister_socket(worker->socket);

    heap_update_node_key(worker_heap, &worker->heap_node, -1);
    heap_pop(worker_heap);

    list_node_delete(list, &worker->list_node);
    list_delete(&worker->client_list);

    free(worker);
}

static void worker_listener_new_max_socket()
{
    int max_socket = 0;
    list_it *it = NULL;
    worker_t *worker = NULL;

    if (!worker_listener || !worker_listener->worker_list) {
        LOG("error: %s() worker_listener not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    list_iterate(worker_listener->worker_list, it) {
        worker = list_info_from_it(it, list_node, worker_t);
        if (worker->socket > max_socket)
            max_socket = worker->socket;
    }

    worker_listener->max_socket = max_socket;
}

void worker_listener_check_worker_sockets(int *num_socks, fd_set *read_sockets)
{
    int rc = 0;
    worker_t *worker = NULL;
    list_it *list_it = NULL;
    list_t *worker_list = NULL;

    if (!num_socks || !read_sockets) {
        LOG("error: %s invalid parameters.", __FUNCTION__);
        clean_exit(-1);
    }

    worker_list = worker_listener_get_worker_list();
    list_iterate(worker_list, list_it) {
        worker = list_info_from_it(list_it, list_node, worker_t);

        if (FD_ISSET(worker->socket, read_sockets)) {
            char buffer[MAX_MESSAGE_SIZE] = {0};

            rc = utils_receive_message_from_socket(worker->socket, (header_t *)buffer);
            if (rc == -1) {
                LOG("worker_listener: %s() error on receiving from %s.",
                    __FUNCTION__, worker->ip_addr);
                worker_listener_free_worker(worker);
                return;
            } else if (rc == 1) {
                LOG("worker_listener: %s closed connection.",
                    worker->ip_addr);
                worker_listener_free_worker(worker);
                return;
            }

            connections_process_message(worker,
                                        (header_t *)buffer,
                                        worker->ip_addr);
            (*num_socks)--;
            if (*num_socks == 0)
                return;
        }
    }
}

int worker_listener_send_build_order(worker_t* worker,
                                     client_t* client,
                                     build_req_msg_t* build_req)
{
    int rc = 0;
    build_order_msg_t order_msg = {0};

    if (!worker || !client || !build_req) {
        LOG("error: %s() invalid parameters.", __FUNCTION__);
        return -1;
    }

    rc = client_listener_get_client_addr(client, &order_msg.client_addr);
    if (rc == -1) {
        LOG("error: %s() couldn't get client address.");
        return -1;
    }

    order_msg.request = *build_req;

    rc = utils_send_message(worker->socket,
                            BUILD_ORDER,
                            sizeof(build_order_msg_t),
                            (char *)&order_msg);
    if (rc == -1) {
        LOG("error: %s() could not send message to %s.",
            __FUNCTION__,
            worker->ip_addr);
        return -1;
    }

    LOG("worker_listener: sent WORKER_BUILD_ORDER to %s",
        worker->ip_addr);
    return rc;
}

void worker_listener_add_worker_to_heap(worker_t *worker)
{
    if (!worker_heap) {
        LOG("error: %s() worker_heap not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    heap_push(worker_heap, &worker->heap_node);
}

worker_t *worker_listener_get_worker_from_heap()
{
    worker_t *worker = NULL;
    heap_node_t *heap_node = NULL;

    if (!worker_heap) {
        LOG("error: %s() worker_heap not initialized.", __FUNCTION__);
        return NULL;
    }

    heap_node = heap_pop(worker_heap);
    if (heap_node)
        worker = heap_info_from_node(heap_node, heap_node, worker_t);

    if (!worker)
        LOG("worker_listener: no worker registered.");

    return worker;
}

void worker_listener_increment_builds_count(worker_t *worker)
{
    if (!worker_heap) {
        LOG("error: %s() worker_heap not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    if (!worker) {
        LOG("error: %s() invalid parameter.", __FUNCTION__);
        return;
    }

    worker->no_current_builds++;
    worker->heap_node.heap_key = worker->no_current_builds;
}

void worker_listener_decrement_builds_count(worker_t *worker)
{
    if (!worker_heap) {
        LOG("error: %s() worker_heap not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    if (!worker) {
        LOG("error: %s() invalid parameter.", __FUNCTION__);
        return;
    }

    worker->no_current_builds--;
    heap_update_node_key(worker_heap, &(worker->heap_node), worker->no_current_builds);
}

static list_t* worker_listener_get_worker_list()
{
    if (!worker_listener || !(worker_listener->worker_list)) {
        LOG("error: %s() worker_listener not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    return worker_listener->worker_list;
}

void worker_listener_add_client_to_list(worker_t *worker, client_t *client)
{
    if (!worker || !client) {
        LOG("error: %s() invalid parameters.", __FUNCTION__);
        return;
    }

    client_listener_add_client_to_list(worker->client_list, client);
}

const char *worker_listener_get_ip_addr(worker_t *worker)
{
    if (!worker) {
        LOG("error: %s() invalid parameter.");
        return NULL;
    }

    return worker->ip_addr;
}

client_t *worker_listener_get_client_from_address(worker_t *worker,
                                                  struct sockaddr_in *client_addr)
{
    client_t *client = NULL;

    if (!worker || !client_addr) {
        LOG("error: %s() invalid arguments.", __FUNCTION__);
        return NULL;
    }

    client = client_listener_get_client_from_address(worker->client_list, client_addr);
    if (!client) {
        LOG("error: %s() could not get client.", __FUNCTION__);
        return NULL;
    }

    return client;
}

int worker_listener_get_max_socket()
{
    if (!worker_listener) {
        LOG("error: %s() worker_listener not initialized.");
        clean_exit(-1);
    }

    return worker_listener->max_socket;
}
