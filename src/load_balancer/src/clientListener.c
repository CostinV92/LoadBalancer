#include <stdlib.h>
#include <stdio.h>

#include <netdb.h>
#include <netinet/in.h>

#include <ctype.h>

#include <string.h>

#include <pthread.h>

#include "utils.h"
#include "secretary.h"

#include "clientListener.h"

static void* start_server(void*);
static void create_server();

client_listener_t *client_listener;

void create_client_listener();

void init_client_listener()
{
    int socket = 0;

    client_listener = malloc(sizeof(client_listener_t));
    if (!client_listener) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    create_client_listener();
}

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
            LOG("Client listener: client connected, ip: %s", format_ip_addr(&client_addr));

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

void register_client(connections_t *connections,
                     int client_socket,
                     struct sockaddr_in *client_addr)
{
    client_t* client = calloc(1, sizeof(client_t));
    if (!client) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    client->socket = client_socket;
    client->addr = *client_addr;

    /* TODO(victor): this doesn't work use a list */
    connections->client_sockets[connections->num_client_sockets++] = client_socket;
    FD_SET(client_socket, &connections->sockets);
    if (client_socket > connections->max_socket)
        connections->max_socket = client_socket;

    LOG("Client: %s registered.", format_ip_addr(client_addr));
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
