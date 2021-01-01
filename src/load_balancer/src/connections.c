#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>

#include "connections.h"

#include "libutils.h"
#include "client_listener.h"
#include "worker_listener.h"

typedef struct connections {
    fd_set              sockets;
    int                 max_socket;

    client_listener_t   *client_listener;
    worker_listener_t   *worker_listener;
} connections_t;

extern client_listener_t *client_listener;
extern worker_listener_t *worker_listener;

extern void clean_exit(int status);

static void connections_listen();
static void connections_register_client(int client_socket,
                                        struct sockaddr_in *client_addr);
static void connections_register_worker(int worker_socket,
                                        struct sockaddr_in *worker_addr);
static void connections_process_build_req(client_t* client, build_req_msg_t* message);
static void connections_process_build_done(worker_t* worker,
                                           build_order_done_msg_t* message);

connections_t connections;

void connections_start_listening()
{
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
    connections_listen(&connections);
}

void connections_stop_listening()
{
    if (connections.worker_listener)
        worker_listener_destroy();

    if (connections.client_listener)
        client_listener_destroy();
}

static void connections_listen()
{
    int rc = 0;
    int worker_socket = 0;
    int client_socket = 0;
    int addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in addr = {0};
    fd_set read_sockets;

    while (1) {
        struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
        read_sockets = connections.sockets;
        rc = select(connections.max_socket + 1, &read_sockets, NULL, NULL, &tv);
        if (rc == -1) {
            LOG("Error: %s() error on select().", __FUNCTION__);
            clean_exit(-1);
        } else if (rc) {
            if (FD_ISSET(connections.client_listener->socket, &read_sockets)) {
                client_socket = accept(connections.client_listener->socket,
                                       (struct sockaddr*)&addr,
                                       &addr_len);

                if (client_socket == -1) {
                    LOG("Error: %s() error accepting client socket.", __FUNCTION__);
                    clean_exit(-1);
                } else {
                    connections_register_client(client_socket, &addr);
                }

                rc--;
                if (rc == 0)
                    continue;
            }

            if (FD_ISSET(connections.worker_listener->socket, &read_sockets)) {
                worker_socket = accept(connections.worker_listener->socket,
                                       (struct sockaddr*)&addr,
                                       &addr_len);

                if (worker_socket == -1) {
                    LOG("Error: %s() error accepting worker socket.", __FUNCTION__);
                    clean_exit(-1);
                } else {
                    connections_register_worker(worker_socket, &addr);
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

static void connections_register_client(int client_socket, struct sockaddr_in *client_addr)
{
    client_t *client = NULL;

    if (!client_socket || !client_addr) {
        LOG("Error: %s() invalid arguments.", __FUNCTION__);
        return;
    }

    client = client_listener_new_client(client_socket, client_addr);
    FD_SET(client_socket, &connections.sockets);

    if (client_socket > connections.max_socket)
        connections.max_socket = client_socket;

    LOG("connections: client %s registered.", client_listener_get_ip_addr(client));
}

static void connections_register_worker(int worker_socket, struct sockaddr_in *worker_addr)
{
    worker_t *worker = NULL;

    if (!worker_socket || !worker_addr) {
        LOG("Error: %s() invalid arguments.", __FUNCTION__);
        return;
    }

    worker = worker_listener_new_worker(worker_socket, worker_addr);
    FD_SET(worker_socket, &connections.sockets);
    if (worker_socket > connections.max_socket)
        connections.max_socket = worker_socket;

    LOG("connections: worker %s registered.", worker_listener_get_ip_addr(worker));
}

void connections_unregister_socket(int socket)
{
    int client_max_socket = 0;
    int worker_max_socket = 0;

    if (!socket) {
        LOG("connections: %s() invalid parameter.", __FUNCTION__);
        return;
    }

    if (socket == connections.max_socket) {
        client_max_socket = client_listener_get_max_socket();
        worker_max_socket = worker_listener_get_max_socket();

        if (client_max_socket > worker_max_socket)
            connections.max_socket = client_max_socket;
        else
            connections.max_socket = worker_max_socket;
    }

    FD_CLR(socket, &connections.sockets);
    close(socket);
}

void connections_process_message(void* peer, header_t* message, char* ip_addr)
{
    message_type_t msg_type = message->type;

    switch (msg_type) {
        case BUILD_REQ:
            LOG("client_listener: from %s got BUILD_REQ", ip_addr);
            connections_process_build_req(peer, (build_req_msg_t *)(message->buffer));
            break;

        case BUILD_DONE:
            LOG("worker_listener: from %s got BUILD_DONE", ip_addr);
            connections_process_build_done(peer,
                                           (build_order_done_msg_t *)(message->buffer));
            break;

        default:
            LOG("WARNING Procces message: Unknown message from  ip: %s", ip_addr);
            break;
    }
}

static void connections_process_build_req(client_t* client, build_req_msg_t* message)
{
    int rc = 0;
    int res = 0;
    worker_t *worker = NULL;

    if (!client || !message) {
        LOG("error: %s() invalid parameters.", __FUNCTION__);
        goto exit_error;
    }

    worker = worker_listener_get_worker_from_heap();
    if (!worker) {
        LOG("connections: couldn't get any worker.");
        goto exit_error;
    }

    rc = worker_listener_send_build_order(worker, client, message);
    if (rc == -1) {
        LOG("connections: couldn't send build order");
        goto exit_error;
    }

    worker_listener_increment_builds_count(worker);
    worker_listener_add_client_to_list(worker, client);
    worker_listener_add_worker_to_heap(worker);
    client_listener_add_worker_to_client(client, worker);

    return;

exit_error:
    client_listener_send_build_res(client, -1, 0);
}

static void connections_process_build_done(worker_t* worker,
                                           build_order_done_msg_t* message)
{
    int status = 0;
    int reason = 0;

    if (!worker || !message) {
        LOG("error: %s() invalid parameters.", __FUNCTION__);
        return;
    }

    status = message->status;
    reason = message->reason;

    client_t *client =
            worker_listener_get_client_from_address(worker,
                                                    &message->build_order.client_addr);

    if (!client) {
        LOG("connections: couldn't find client");
        return;
    }

    client_listener_send_build_res(client, status, reason);
    worker_listener_decrement_builds_count(worker);
}
