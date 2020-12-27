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

#include "workerListener.h"

static void create_server();
static void* start_server(void*);
#if 0 /* TODO(victor): rework */
static void* register_worker(void*);
#endif
static bool get_worker_hostname(worker_t*);
static void listen_to_worker(worker_t*);


/***************************************************************/
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
            LOG("Worker listener: worker connected, ip: %s", format_ip_addr(&worker_addr));

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

    heap_push(worker_heap, &worker->heap_node);

    LOG("Worker: new worker %s.", format_ip_addr(worker_addr));
}

#if 0 /* TODO(victor) rework */
void* register_worker(void* arg)
{
    char service[20] = {0};
    worker_t *worker = (worker_t*)arg;

    if (getnameinfo((struct sockaddr *)&(worker->addr), sizeof(worker->addr), worker->hostname, sizeof(worker->hostname), service, sizeof(service), 0) == 0) {
        heap_push(worker_heap, &(worker->heap_node));
        LOG("Worker listener: worker added to database, hostname: %s, ip: %s", worker->hostname, format_ip_addr(&(worker->addr)));
    } else {
        LOG("ERROR Worker listener: failed to add worker to database (no hostname), ip: %s", format_ip_addr(&(worker->addr)));
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
            process_message(worker, (void*)buffer, worker->hostname, format_ip_addr(&(worker->addr)));
        } else {
            LOG("%x Worker listener: worker disconnected, hostname: %s, ip: %s", worker, worker->hostname, format_ip_addr(&(worker->addr)));
            worker->alive = false;
            break;
        }
    }
}
