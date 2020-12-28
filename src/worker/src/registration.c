#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "registration.h"
#include "utils.h"

static void connect_to_server();
static void get_lb_address();

char lb_address[20];

void register_worker()
{
    // connect to the load balancer and anounce yourself
    loadBalancer = calloc(1, sizeof(load_balancer_server_t));
    if (!loadBalancer) {
        LOG("Error: %s() cannot alloc memory!", __FUNCTION__);
        clean_exit();
    }

    get_lb_address();
    connect_to_server();
}

void get_lb_address()
{
    char *env;
    env = getenv("LOAD_BALANCER_ADDRESS");
    if (!env) {
        LOG("Error: %s() LOAD_BALANCER_ADDRESS env variable doesn't exist", __FUNCTION__);
        clean_exit();
    } else {
        strcpy(lb_address, env);
    }
}

void connect_to_server()
{
    int server_socket = 0, port = 0, iSetOption = 1;
    struct sockaddr_in server_addr;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        LOG("Error: %s() cannot create socket to load balancer.", __FUNCTION__);
        clean_exit();
    }
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));


    // init address structure
    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    port = SERVER_PORT;
    server_addr.sin_family = AF_INET;

    server_addr.sin_addr.s_addr = inet_addr(lb_address);
    server_addr.sin_port = htons(port);

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
        LOG("Error: %s() cannot connect to load balancer.", __FUNCTION__);
        clean_exit();
    }

    loadBalancer->socket = server_socket;
    loadBalancer->server = server_addr;

    LOG("Connected to LoadBalancer");
}
