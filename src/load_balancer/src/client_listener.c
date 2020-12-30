#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include "client_listener.h"

#include "messages.h"
#include "libutils.h"
#include "connections.h"

struct client {
    int                     socket;
    struct sockaddr_in      addr;
    char                    ip_addr[MAX_IP_ADDR_SIZE];
    list_node_t             list_node;
    list_node_t             list_worker_node;
};

client_listener_t *client_listener;

extern void clean_exit(int status);

static void client_listener_create();
static void client_listener_free_client(client_t *client);
static void client_listener_new_max_socket();
static list_t* client_listener_get_client_list();

void client_listener_init()
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

    client_listener_create();
}

void client_listener_destroy()
{
    client_t *client = NULL;
    list_it *it = NULL;

    if (!client_listener || !client_listener->client_list)
        return;

    list_iterate(client_listener->client_list, it) {
        client = list_info_from_it(it, list_node, client_t);

        list_node_delete(client_listener->client_list, &client->list_node);

        client_listener_free_client(client);
    }

    list_delete(&client_listener->client_list);
    close(client_listener->socket);
    free(client_listener);
}

static void client_listener_create()
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

client_t *client_listener_new_client(int client_socket,
                                     struct sockaddr_in *client_addr)
{
    if (!client_listener) {
        LOG("Error: %s() client_listener not initialized.");
        return NULL;
    }

    client_t* client = calloc(1, sizeof(client_t));
    if (!client) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    client->socket = client_socket;
    client->addr = *client_addr;
    utils_format_ip_addr(client_addr, client->ip_addr);

    if (client_socket > client_listener->max_socket)
        client_listener->max_socket = client_socket;

    list_node_init(&client->list_node);
    list_add_back(client_listener->client_list, &client->list_node);

    LOG("client_listener: new client %s", client->ip_addr);

    return client;
}

static void client_listener_free_client(client_t *client)
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

    if (client->socket == client_listener->max_socket)
        client_listener_new_max_socket();

    connections_unregister_socket(client->socket);

    list_node_delete(list, &client->list_node);
    free(client);
}

static void client_listener_new_max_socket()
{
    int max_socket = 0;
    list_it *it = NULL;
    client_t *client = NULL;

    if (!client_listener || !client_listener->client_list) {
        LOG("error: %s() client_listener not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    list_iterate(client_listener->client_list, it) {
        client = list_info_from_it(it, list_node, client_t);
        if (client->socket > max_socket)
            max_socket = client->socket;
    }

    client_listener->max_socket = max_socket;
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
        client = list_info_from_it(list_it, list_node, client_t);

        if (FD_ISSET(client->socket, read_sockets)) {
            char buffer[MAX_MESSAGE_SIZE] = {0};

            rc = utils_receive_message_from_socket(client->socket, (header_t *)buffer);
            if (rc == -1) {
                LOG("client_listener: %s() error on receiving from %s.",
                    __FUNCTION__, client->ip_addr);
                client_listener_free_client(client);
                return;
            } else if (rc == 1) {
                LOG("client_listener: %s closed connection.",
                    client->ip_addr);
                client_listener_free_client(client);
                return;
            }

            connections_process_message(client,
                                        (header_t *)buffer,
                                        client->ip_addr);
            (*num_socks)--;
            if (*num_socks == 0)
                return;
        }
    }
}

int client_listener_send_build_res(client_t* client, int status, int reason)
{
    int rc = 0;
    build_res_msg_t res_msg = {0};

    if (!client) {
        LOG("error: %s() invalid parameter.");
        return -1;
    }

    res_msg.status = status;
    res_msg.reason = reason;

    rc = utils_send_message(client->socket,
                            BUILD_RES,
                            sizeof(build_res_msg_t),
                            (char *)&res_msg);
    if (rc == -1) {
        LOG("error: %s() could not send message to %s.",
            __FUNCTION__,
            client->ip_addr);
        return -1;
    }

    LOG("client_listener: sent BUILD_RES to %s", client->ip_addr);

    return rc;
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

const char *client_listener_get_ip_addr(client_t *client)
{
    if (!client) {
        LOG("error: %s() invalid parameter.");
        return NULL;
    }

    return client->ip_addr;
}

static list_t* client_listener_get_client_list()
{
    if (!client_listener || !(client_listener->client_list)) {
        LOG("error: %s() client_listener not initialized.", __FUNCTION__);
        clean_exit(-1);
    }

    return client_listener->client_list;
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
        client = list_info_from_it(it, list_worker_node, client_t);
        if (memcmp(&client->addr, client_addr, sizeof(struct sockaddr_in)) == 0) {
            list_node_delete(list, &client->list_worker_node);
            return client;
        }
    }

    return NULL;
}

int client_listener_get_client_addr(client_t *client, struct sockaddr_in *client_address)
{
    if (!client || !client_address) {
        LOG("client_listener: %s() invalid paramameter.", __FUNCTION__);
        return -1;
    }

    memcpy(client_address, &client->addr, sizeof(struct sockaddr_in));
    return 0;
}

int client_listener_get_max_socket()
{
    if (!client_listener) {
        LOG("error: %s() client_listener not initialized.");
        clean_exit(-1);
    }

    return client_listener->max_socket;
}
