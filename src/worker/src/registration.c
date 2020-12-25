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
    get_lb_address();
    connect_to_server();
}

void get_lb_address()
{
    char *env;
    env = getenv("LOAD_BALANCER_ADDRESS");
    if (!env) {
        FILE *f = fopen("/vagrant/lb_address", "r");
        if (f) {
            fgets(lb_address, sizeof(lb_address), f);
            fclose(f);
        } else {
            LOG("Don't know server address");
            exit(1);
        }
    } else {
        strcpy(lb_address, env);
    }
}

void connect_to_server()
{
    int server_socket, port, iSetOption = 1;
    struct sockaddr_in server_addr;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

    if (server_socket < 0) {
        LOG("ERROR opening LoadBalancer socket");
        exit(1);    
    }

    // init address structure
    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    port = SERVER_PORT;

    server_addr.sin_family = AF_INET;
    // TODO: take the real server addr
    server_addr.sin_addr.s_addr = inet_addr(lb_address);
    server_addr.sin_port = htons(port);

    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        LOG("ERROR connecting to LoadBalancer");
        exit(1);
    }

    loadBalancer->socket = server_socket;
    loadBalancer->server = server_addr;

    LOG("Connected to LoadBalancer");
}