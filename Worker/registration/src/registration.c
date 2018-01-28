#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "registration.h"
#include "utils.h"

static void connect_to_server();

void register_worker()
{
    // connect to the load balancer and anounce yourself
    loadBalancer = calloc(1, sizeof(load_balancer_server_t));
    connect_to_server();
}

void connect_to_server()
{
    int server_socket, port;
    struct sockaddr_in server_addr;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0) {
        perror("ERROR opening LoadBalancer socket");
        exit(1);    
    }

    // init address structure
    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    port = SERVER_PORT;

    server_addr.sin_family = AF_INET;
    // TODO: take the real server addr
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("ERROR connecting to LoadBalancer");
        exit(1);
    }

    loadBalancer->socket = server_socket;
    loadBalancer->server = server_addr;

    LOG("Connected to LoadBalancer");
}