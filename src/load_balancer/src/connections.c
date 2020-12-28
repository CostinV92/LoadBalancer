#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>

#include "connections.h"
#include "messages.h"
#include "utils.h"
#include "libutils.h"
#include "client_listener.h"
#include "worker_listener.h"

extern heap_t *worker_heap;

bool send_build_res(client_t*, bool, int);

static void* listen_work_done(void*);

/***********************************************************************/
extern client_listener_t *client_listener;
extern worker_listener_t *worker_listener;

static void listen_connections(connections_t *connections);

void start_listening()
{
    connections_t connections = {0};

    if (!client_listener) {
        LOG("Error: %s() client_listener not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    if (!worker_listener) {
        LOG("Error: %s() worker_listener not initialized.", __FUNCTION__);
    }

    connections.max_socket = client_listener->socket > worker_listener->socket ?
                                    client_listener->socket : worker_listener->socket;
    FD_ZERO(&connections.sockets);
    FD_SET(client_listener->socket, &connections.sockets);
    FD_SET(worker_listener->socket, &connections.sockets);

    connections.client_listener = client_listener;
    connections.worker_listener = worker_listener;

    LOG("Starting listening...");
    listen_connections(&connections);
}

static void register_client(connections_t *connections,
                            int client_socket,
                            struct sockaddr_in *client_addr)
{
    if (!connections || !client_socket || !client_addr) {
        LOG("Error: %s() invalid arguments.", __FUNCTION__);
        return;
    }

    client_listener_new_client(client_socket, client_addr);
    FD_SET(client_socket, &connections->sockets);

    if (client_socket > connections->max_socket)
        connections->max_socket = client_socket;

    LOG("Client: %s registered.", utils_format_ip_addr(client_addr));
}

static void register_worker(connections_t *connections,
                            int worker_socket,
                            struct sockaddr_in *worker_addr)
{
    if (!connections || !worker_socket || !worker_addr) {
        LOG("Error: %s() invalid arguments.", __FUNCTION__);
        return;
    }

    worker_listener_new_worker(worker_socket, worker_addr);
    FD_SET(worker_socket, &connections->sockets);
    if (worker_socket > connections->max_socket)
        connections->max_socket = worker_socket;

    LOG("Worker: %s registered.", utils_format_ip_addr(worker_addr));
}

static void listen_connections(connections_t *connections)
{
    int rc = 0;
    int worker_socket = 0;
    int client_socket = 0;
    int addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in addr = {0};
    fd_set read_sockets;

    while (1) {
        struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
        read_sockets = connections->sockets;
        rc = select(connections->max_socket + 1, &read_sockets, NULL, NULL, &tv);
        if (rc == -1) {
            LOG("Error: %s() error on select().", __FUNCTION__);
            clean_exit(-1);
        } else if (rc) {
            if (FD_ISSET(connections->client_listener->socket, &read_sockets)) {
                client_socket = accept(connections->client_listener->socket,
                                       (struct sockaddr*)&addr,
                                       &addr_len);

                if (client_socket == -1) {
                    LOG("Error: %s() error accepting client socket.", __FUNCTION__);
                    clean_exit(-1);
                } else {
                    register_client(connections, client_socket, &addr);
                }

                rc--;
                if (rc == 0)
                    continue;
            }

            if (FD_ISSET(connections->worker_listener->socket, &read_sockets)) {
                worker_socket = accept(connections->worker_listener->socket,
                                       (struct sockaddr*)&addr,
                                       &addr_len);

                if (worker_socket == -1) {
                    LOG("Error: %s() error accepting worker socket.", __FUNCTION__);
                    clean_exit(-1);
                } else {
                    register_worker(connections, worker_socket, &addr);
                }

                rc--;
                if (rc == 0)
                    continue;
            }

            client_listener_check_client_sockets(&rc, &read_sockets);
            if (rc == 0)
                continue;

            worker_listener_check_worker_sockets(&rc, &read_sockets);
            if (rc == 0)
                continue;
        }
    }
}

#if 0
void* assign_secretary(void* arg)
{
    client_t *client = (client_t*)arg;
    char service[20] = {0};

    if (getnameinfo((struct sockaddr *)&(client->addr), sizeof(client->addr),
        client->hostname, sizeof(client->hostname), service, sizeof(service), 0) == 0) {
        LOG("Secretary: Client introduced himself, hostname: %s, ip: %s",
            client->hostname, utils_format_ip_addr(&(client->addr)));
    } else {
        LOG("Warning: Client didn't introduced himself, ip: %s",
            utils_format_ip_addr(&(client->addr)));
    }

    for (;;) {
        int byte_read;
        char buffer[2 * 256] = {0};
        byte_read = read(client->socket, buffer, sizeof(buffer));
        if (byte_read > 0) {
            // TODO: DEBUG
            process_message(client, (message_t*)buffer,
                client->hostname, utils_format_ip_addr(&(client->addr)));
        } else {
            LOG("Secretary: Client closed connection, hostname: %s, ip: %s",
                client->hostname, utils_format_ip_addr(&(client->addr)));

            close(client->socket);
            free(client);
            break;
        }
    }
}
#endif

void process_build_done(worker_t* worker, build_order_done_msg_t* message)
{
    int status = message->status;
    int reason = message->reason;
    client_t *client = worker_listener_get_client(worker);

    /* TODO(victor): client_t not known */
    /*if (!status) {
        LOG("Warning: Build failed with reason: %d on hostname: %s, ip: %s, for hostname: %s, ip: %s",
            reason, worker->hostname, utils_format_ip_addr(&(worker->addr)),
                client->hostname, utils_format_ip_addr(&(client->addr)));
    } else {
        LOG("Secretary: Build succeded on hostname: %s, ip: %s, for hostname: %s, ip: %s",
            worker->hostname, utils_format_ip_addr(&(worker->addr)),
                client->hostname, utils_format_ip_addr(&(client->addr)));
    }*/

    send_build_res(client, status, reason);
    worker_listener_decrement_no_of_builds_and_update_node_key(worker);
}