#include <stdlib.h>
#include <stdio.h>

#include <netdb.h>
#include <netinet/in.h>

#include <ctype.h>

#include <string.h>

#include <pthread.h>

#include "utils.h"
#include "secretary.h"
#include "messages.h"

#include "clientListener.h"

#include "libutils.h"

#define MAX_MESSAGE_SIZE 512

struct client {
    int                     socket;
    struct sockaddr_in      addr;
    list_node_t             list_node;
};

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

#if 0
void* start_server(void* arg) 
{
    int iSetOption = 1;
    int client_socket, client_len = sizeof(struct sockaddr_in), *socket = (int*)arg;
    struct sockaddr_in client_addr;
    pthread_t secretary_thread_id;

    while (1) {
        client_socket = accept(*socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            LOG("Error: %s() error on accepting connection.", __FUNCTION__);
            //this is for reusing the port imeddiatly after error
            setsockopt(*socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
            clean_exit(-1);
        } else {
            LOG("Client listener: client connected, ip: %s", utils_format_ip_addr(&client_addr));

            client_t* client = calloc(1, sizeof(client_t));
            if (!client) {
                LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
                clean_exit(-1);
            }

            client->socket = client_socket;
            client->addr = client_addr;

            pthread_create(&secretary_thread_id, NULL, &assign_secretary, client);
        }
    }
}
#endif

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

    list_node_delete(list, &client->list_node);
    free(client);
}

void create_client_listener()
{
    int server_socket, port, iSetOption = 1;
    struct sockaddr_in server;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

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

void client_listener_message_from_client(client_t *client)
{
    int rc = 0;
    int header_size = 0;
    int payload_size = 0;
    char buffer[MAX_MESSAGE_SIZE] = {0};

    if (!client) {
        LOG("error: %s() invalid client.");
        clean_exit(-1);
    }

    header_size = sizeof(message_t);
    rc = utils_receive_message_from_socket(client->socket, buffer, header_size);
    if (rc == 0) {
        payload_size = ((message_t*)buffer)->size;
    } else if (rc == 1) {
        LOG("client_listener: %s closed connection.", utils_format_ip_addr(&(client->addr)));
        client_listener_free_client(client);
        return;
    } else {
        LOG("error: %s() error on receiving from socket.", __FUNCTION__);
        client_listener_free_client(client);
        return;
    }

    rc = utils_receive_message_from_socket(client->socket,
                                           buffer + header_size,
                                           payload_size);
    if (rc == 0) {
        process_message(client, (message_t *)buffer, utils_format_ip_addr(&(client->addr)));
    } else if (rc == 1) {
        LOG("client_listener: %s closed connection.", utils_format_ip_addr(&(client->addr)));
        client_listener_free_client(client);
        return;
    } else {
        LOG("error: %s() error on receiving from socket.", __FUNCTION__);
        client_listener_free_client(client);
        return;
    }
}

void client_listener_check_client_sockets(int *num_socks, fd_set *read_sockets)
{
    client_t *client= NULL;
    list_it *list_it = NULL;
    list_t *client_list = NULL;

    if (!num_socks || !read_sockets) {
        LOG("error: %s invalid parameter.", __FUNCTION__);
        clean_exit(-1);
    }

    client_list = client_listener_get_client_list();
    list_iterate(client_list, list_it) {
        client = info_from_it(list_it, list_node, client_t);

        if (FD_ISSET(client->socket, read_sockets)) {
            client_listener_message_from_client(client);

            (*num_socks)--;
            if (num_socks == 0)
                return;
        }
    }
}

#if 0
void* assign_secretary(void* arg)
{
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

bool send_build_res(client_t* client, bool status, int reason)
{
    build_res_msg_t res_msg;

    res_msg.status = status;
    res_msg.reason = reason;

    if (send_message(client->socket, SECRETARY_BUILD_RES, sizeof(build_res_msg_t), (char*)&res_msg) < 0) {
        //here we will return, the unconnected client will be logged by the read in assign secretary
        return false;
    }

    LOG("Send message: Send message SECRETARY_BUILD_RES to client ip: %s",
        utils_format_ip_addr(&(client->addr)));
    return true;
}

struct sockaddr_in client_listener_get_client_addr(client_t *client)
{
    return client->addr;
}
