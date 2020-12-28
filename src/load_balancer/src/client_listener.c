#include <stdlib.h>
#include <stdio.h>

#include <netdb.h>
#include <netinet/in.h>

#include <ctype.h>

#include <string.h>

#include <pthread.h>

#include "connections.h"
#include "messages.h"

#include "client_listener.h"

#include "libutils.h"

#define MAX_MESSAGE_SIZE 512

struct client {
    int                     socket;
    struct sockaddr_in      addr;
    list_node_t             list_node;
    list_node_t             list_worker_node;
};

extern void clean_exit(int status);

static void* start_server(void*);
static void create_server();

client_listener_t *client_listener;

void create_client_listener();

void init_client_listener()
{
    int socket = 0;

    client_listener = calloc(1, sizeof(client_listener_t));
    if (!client_listener) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    client_listener->client_list = list_new();
    if (!client_listener->client_list) {
        LOG("Error: %s() cannot allocate client list.", __FUNCTION__);
        clean_exit(-1);
    }

    create_client_listener();
}

list_t* client_listener_get_client_list()
{
    if (!client_listener || !(client_listener->client_list)) {
        LOG("error: %s() client_listener not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    return client_listener->client_list;
}

void client_listener_new_client(int client_socket,
                                struct sockaddr_in *client_addr)
{
    if (!client_listener) {
        LOG("Error: %s() client_listener not initialized.");
        return;
    }

    client_t* client = calloc(1, sizeof(client_t));
    if (!client) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    client->socket = client_socket;
    client->addr = *client_addr;

    list_node_init(&client->list_node);
    list_add_back(client_listener->client_list, &client->list_node);

    LOG("client_listener: new client %s.", utils_format_ip_addr(client_addr));
}

void client_listener_free_client(client_t *client)
{
    list_t *list = NULL;

    if (!client) {
        LOG("error: %s invalid client.", __FUNCTION__);
        return;
    }

    list = client_listener_get_client_list();
    if (!list) {
        LOG("error: %s client list not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    connections_unregister_socket(client->socket);

    list_node_delete(list, &client->list_node);
    free(client);
}

void create_client_listener()
{
    int server_socket, port, iSetOption = 1;
    struct sockaddr_in server;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_socket,
               SOL_SOCKET,
               SO_REUSEADDR,
               (char*)&iSetOption,
               sizeof(iSetOption));

    if (server_socket == -1) {
        LOG("Error: %s() on opening client listener socket.", __FUNCTION__);
        clean_exit(-1);
    }

    // init address structure
    memset(&server, 0, sizeof(server));

    port = CLIENT_PORT;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // bind the socket with the address
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0 ) {
        LOG("Error: %s() on biding client listener socket.", __FUNCTION__);
        clean_exit(-1);
    }

    // mark socket as pasive socket
    if (listen(server_socket, 20) == -1) {
        LOG("Error: %s() on marking client listener socket as passive.", __FUNCTION__);
        clean_exit(-1);
    }

    // save listener info
    client_listener->socket = server_socket;
    client_listener->server = server;
}

void client_listener_check_client_sockets(int *num_socks, fd_set *read_sockets)
{
    int rc = 0;
    client_t *client= NULL;
    list_it *list_it = NULL;
    list_t *client_list = NULL;

    if (!num_socks || !read_sockets) {
        LOG("error: %s invalid parameters.", __FUNCTION__);
        clean_exit(-1);
    }

    client_list = client_listener_get_client_list();
    list_iterate(client_list, list_it) {
        client = info_from_it(list_it, list_node, client_t);

        if (FD_ISSET(client->socket, read_sockets)) {
            char buffer[MAX_MESSAGE_SIZE] = {0};

            rc = utils_receive_message_from_socket(client->socket, buffer);
            if (rc == -1) {
                LOG("worker_listener: %s() error on receiving from %s.",
                    __FUNCTION__, utils_format_ip_addr(&client->addr));
                client_listener_free_client(client);
                return;
            } else if (rc == 1) {
                LOG("worker_listener: %s closed connection.",
                    utils_format_ip_addr(&client->addr));
                client_listener_free_client(client);
                return;
            }

            connections_process_message(client,
                                        (header_t *)buffer,
                                        utils_format_ip_addr(&client->addr));
            (*num_socks)--;
            if (*num_socks == 0)
                return;
        }
    }
}

int send_build_res(client_t* client, int status, int reason)
{
    build_res_msg_t res_msg;

    res_msg.status = status;
    res_msg.reason = reason;

    if (send_message(client->socket,
                     SECRETARY_BUILD_RES,
                     sizeof(build_res_msg_t),
                     (char*)&res_msg) < 0) {
        //here we will return, the unconnected client will be logged by the read in assign secretary
        return 0;
    }

    LOG("Send message: Send message SECRETARY_BUILD_RES to client ip: %s",
        utils_format_ip_addr(&(client->addr)));
    return 1;
}

struct sockaddr_in client_listener_get_client_addr(client_t *client)
{
    return client->addr;
}

void client_listener_add_client_to_list(list_t *list, client_t *client)
{
    if (!list || !client) {
        LOG("error: %s() invalid arguments.", __FUNCTION__);
        return;
    }

    list_node_init(&client->list_worker_node);
    list_add_back(list, &client->list_worker_node);
}

client_t *client_listener_get_client_from_address(list_t *list,
                                                  struct sockaddr_in *client_addr)
{
    client_t *client = NULL;
    list_it *it = NULL;

    if (!list || !client_addr) {
        LOG("error: %s() invalid arguments.");
        return NULL;
    }

    list_iterate(list, it) {
        client = info_from_it(it, list_worker_node, client_t);
        if (memcmp(&client->addr, client_addr, sizeof(struct sockaddr_in)) == 0) {
            list_node_delete(list, &client->list_worker_node);
            return client;
        }
    }

    return NULL;
}
