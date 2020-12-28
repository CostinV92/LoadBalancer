#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <ctype.h>

#include <string.h>

#include <pthread.h>

#include "utils.h"
#include "libutils.h"

#include "client_listener.h"
#include "worker_listener.h"

static void create_server();
static void* start_server(void*);
#if 0 /* TODO(victor): rework */
static void* register_worker(void*);
#endif
static bool get_worker_hostname(worker_t*);
static void listen_to_worker(worker_t*);


/***************************************************************/

typedef struct worker {
    heap_node_t             heap_node;

    // network info
    int                     socket;
    struct sockaddr_in      addr;
    list_node_t             list_node;

    // worker info
    char                    hostname[256];
    int                     no_current_builds;
    bool                    alive;

    list_t                  *client_list;
} worker_t;

void create_worker_listener();


worker_listener_t *worker_listener;
heap_t *worker_heap;

void init_worker_listener()
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

    create_worker_listener();
}

list_t* worker_listener_get_worker_list()
{
    if (!worker_listener || !(worker_listener->worker_list)) {
        LOG("error: %s() worker_listener not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    return worker_listener->worker_list;
}

#if 0 /* TODO(victor): rework */
void* start_server(void* arg) 
{
    int iSetOption = 1;
    int worker_socket, worker_len = sizeof(struct sockaddr_in), *socket = (int*)arg;
    struct sockaddr_in worker_addr;

    while (1) {
        worker_socket = accept(*socket, (struct sockaddr*)&worker_addr, &worker_len);
        if (worker_socket < 0) {
            LOG("Error: %s() error on accepting worker connection.", __FUNCTION__);
            //this is for reusing the port imeddiatly after error
            setsockopt(*socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
            exit(1);
        } else {
            pthread_t worker_thread_id;
            LOG("Worker listener: worker connected, ip: %s", utils_format_ip_addr(&worker_addr));

            worker_t* worker = calloc(1, sizeof(worker_t));
            if (!worker) {
                LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
                clean_exit(-1);
            }

            worker->socket = worker_socket;
            worker->addr = worker_addr;
            worker->alive = true;
            pthread_mutex_init(&worker->mutex, NULL);

            pthread_create(&worker_thread_id, NULL, &register_worker, worker);
        }
    }
}
#endif

void create_worker_listener()
{
    int server_socket, port, iSetOption = 1;
    struct sockaddr_in server_addr;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        LOG("Error: %s() error on opening worker listener socket.", __FUNCTION__);
        clean_exit(-1);
    }
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

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

void worker_listener_new_worker(int worker_socket,
                                struct sockaddr_in *worker_addr)
{
    if (!worker_listener) {
        LOG("Error: %s() worker_listener not initialized.");
        return;
    }

    worker_t* worker = calloc(1, sizeof(worker_t));
    if (!worker) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    worker->socket = worker_socket;
    worker->addr = *worker_addr;
    worker->alive = true;

    list_node_init(&worker->list_node);
    list_add_back(worker_listener->worker_list, &worker->list_node);

    worker->client_list = list_new();

    heap_push(worker_heap, &worker->heap_node);

    LOG("Worker: new worker %s.", utils_format_ip_addr(worker_addr));
}

void worker_listener_free_worker(worker_t *worker)
{
    /* TODO(victor): to be implemented */
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
        worker = info_from_it(list_it, list_node, worker_t);

        if (FD_ISSET(worker->socket, read_sockets)) {
            char buffer[MAX_MESSAGE_SIZE] = {0};

            rc = utils_receive_message_from_socket(worker->socket, buffer);
            if (rc == -1) {
                LOG("worker_listener: %s() error on receiving from %s.", __FUNCTION__, utils_format_ip_addr(&worker->addr));
                worker_listener_free_worker(worker);
            } else if (rc == 1) {
                LOG("worker_listener: %s closed connection.", utils_format_ip_addr(&worker->addr));
                worker_listener_free_worker(worker);
            }

            process_message(worker, (header_t *)buffer, utils_format_ip_addr(&worker->addr));
            (*num_socks)--;
            if (num_socks == 0)
                return;
        }
    }
}

#if 0 /* TODO(victor) rework */
void* register_worker(void* arg)
{
    char service[20] = {0};
    worker_t *worker = (worker_t*)arg;

    if (getnameinfo((struct sockaddr *)&(worker->addr), sizeof(worker->addr), worker->hostname, sizeof(worker->hostname), service, sizeof(service), 0) == 0) {
        heap_push(worker_heap, &(worker->heap_node));
        LOG("Worker listener: worker added to database, hostname: %s, ip: %s", worker->hostname, utils_format_ip_addr(&(worker->addr)));
    } else {
        LOG("ERROR Worker listener: failed to add worker to database (no hostname), ip: %s", utils_format_ip_addr(&(worker->addr)));
        free(worker);
    }

    listen_to_worker(worker);
}
#endif

void listen_to_worker(worker_t *worker)
{
    int bytes_read;
    char buffer[2 * 256] = {0};

    // wait for done message from the worker
    for (;;) {
        bytes_read = read(worker->socket, buffer, sizeof(buffer));
        if (bytes_read) {
            process_message(worker, (void*)buffer, worker->hostname, utils_format_ip_addr(&worker->addr));
        } else {
            LOG("%x Worker listener: worker disconnected, hostname: %s, ip: %s", worker, worker->hostname, utils_format_ip_addr(&worker->addr));
            worker->alive = false;
            break;
        }
    }
}

client_t *worker_listener_get_client(worker_t *worker, struct sockaddr_in *client_addr)
{
    client_t *client = NULL;

    if (!worker || !client_addr) {
        LOG("error: %s() invalid arguments.", __FUNCTION__);
        return NULL;
    }

    client = client_listener_get_client_with_address(worker->client_list, client_addr);
    if (!client) {
        LOG("error: %s() could not get client.", __FUNCTION__);
        return NULL;
    }

    return client;
}

void process_build_req(client_t* client, build_req_msg_t* message)
{
    bool another_one;
    // get a worker from the specific heap
    heap_t *heap = worker_heap;
    worker_t *worker;
    heap_node_t *heap_node;

    // for responding to the client
    bool status = true;
    int reason = 0;

    do {
        another_one = false;

        status = true;
        reason = 0;


        heap_node = heap_pop(heap);
        if (heap_node)
            worker = INFO(heap_node, worker_t);
        else
            worker = 0;

        if (!worker) {
            LOG("Warning: Don't have any workers available!");
            status = false;
            reason = 1;
            continue;
        }


        if (status && worker->no_current_builds >= 2) {
            LOG("Warning: Best worker already has 2 or more current builds!");
            status = false;
            reason = 2;
            heap_push(heap, &(worker->heap_node));
        }

        if (status && !worker->alive) {
            LOG("Warning: Worker no longer available worker: %s  ip: %s",
                    worker->hostname, utils_format_ip_addr(&worker->addr));
            status = false;
            reason = 3;

            free(worker);
            worker = NULL;

            another_one = true;
            continue;
        }

        if (status && !send_build_order(worker, client, message)) {
            status = false;
            reason = 4;
        }

        if (status) {
            //Here the worker will have begun the build so update the worker info and re add it to the heap
            worker->no_current_builds++;
            /* TODO(victor): resolv this IMIDIATLY */
            client_listener_add_client_to_list(worker->client_list, client);
            worker->heap_node.heap_key = worker->no_current_builds;
            heap_push(heap, &(worker->heap_node));

            /*LOG("Secretary: Worker assigned worker: %s  client: %s",
                worker->hostname, client->hostname);*/
        }

        if (!status && reason == 4) {
            free(worker);
            worker = NULL;
        }
    } while (another_one);

    if (!status)
        send_build_res(client, status, reason);
}

bool send_build_order(worker_t* worker, client_t* client, build_req_msg_t* build_message)
{
    build_order_msg_t build_order;

    build_order.client_addr = client_listener_get_client_addr(client);
    build_order.request = *build_message;

    if (send_message(worker->socket, WORKER_BUILD_ORDER, sizeof(build_order_msg_t), (char*)&build_order) < 0) {
        // return, the unconnected worker will be handled in process_build_req
        return false;
    }

    LOG("Send message: Send message WORKER_BUILD_ORDER to worker hostname: %s, ip: %s",
        worker->hostname, utils_format_ip_addr(&worker->addr));
    return true;
}

void worker_listener_decrement_no_of_builds_and_update_node_key(worker_t *worker)
{
    worker->no_current_builds--;
    heap_update_node_key(worker_heap, &(worker->heap_node), worker->no_current_builds);
}
