#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "utils.h"
#include "messages.h"

typedef struct LOAD_BALANCER {
    int                     socket;
    struct sockaddr_in      server;
} load_balancer_server_t;

typedef struct OUTPUT_LISTENER {
    int                     socket;
    struct sockaddr_in      server;
    pthread_t               thread_id;
} output_listener_t;

load_balancer_server_t  loadBalancer;
output_listener_t       output_listener;
build_res_msg_t         build_res;
char lb_address[20];
int output_listen_port;

void create_server();
void* start_server(void* arg);

void listen_for_output()
{
    int socket;

    create_server();

    pthread_t server_thread_id;
    pthread_create(&server_thread_id, NULL, &start_server, NULL);

    output_listener.thread_id = server_thread_id;
    LOG("Output server created");
}

void get_listen_port()
{
    char *env, port[10];
    env = getenv("LOAD_BALANCER_CLIENT_LISTEN_PORT");
    if (!env) {
        FILE *f = fopen("/vagrant/output_listen_port", "r");
        if (f) {
            fgets(port, sizeof(port), f);
            fclose(f);
        } else {
            LOG("Don't know listen for output port");
            exit(1);
        }
    } else {
        strcpy(port, env);
    }

    output_listen_port = atoi(port);
}

void create_server() 
{
    int server_socket, port, iSetOption = 1;
    struct sockaddr_in server;

    // create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

    if (server_socket < 0) {
        LOG("ERROR opening output listener socket");
        exit(1);    
    }

    // init address structure
    memset(&server, 0, sizeof(struct sockaddr_in));

    get_listen_port();
    port = output_listen_port;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // bind the socket with the address
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) < 0 ) {
        LOG("ERROR on binding output listener socket");
        exit(1);
    }

    // mark socket as pasive socket
    if (listen(server_socket, 20) < 0) {
        LOG("ERROR on marking output listener socket as passive");
        exit(1);
    }

    // save listener info
    output_listener.socket = server_socket;
    output_listener.server = server;
}

void* start_server(void* arg) 
{
    int iSetOption = 1;
    int output_socket, client_len = sizeof(struct sockaddr_in), *socket = (int*)arg;
    struct sockaddr_in worker_addr;

    output_socket = accept(output_listener.socket, (struct sockaddr*)&worker_addr, &client_len);
    if (output_socket < 0) {
        LOG("ERROR on accepting output");
        exit(1);
    } else {
        LOG("Worker connected to output socket, ip: %s", format_ip_addr(&worker_addr));

        for (;;) {
            int byte_read;
            char buffer[2 * 256] = {0};
            byte_read = read(output_socket, buffer, sizeof(buffer));
            if (byte_read > 0) {
                printf("%s", buffer);
            } else {
                close(output_socket);
                break;
            }
        }

        LOG("Worker disconnected from output socket");
    }
}

void connect_to_lb()
{
    int lb_socket, port, iSetOption = 1;
    struct sockaddr_in lb_addr;

    // create the socket
    lb_socket = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(lb_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

    if (lb_socket < 0) {
        LOG("ERROR opening LoadBalancer socket");
        exit(1);
    }

    // init address structure
    memset(&lb_addr, 0, sizeof(struct sockaddr_in));

    // TODO: make it generic    
    port = 7891;

    lb_addr.sin_family = AF_INET;
    lb_addr.sin_addr.s_addr = inet_addr(lb_address);
    lb_addr.sin_port = htons(port);


    if (connect(lb_socket, (struct sockaddr *)&lb_addr, sizeof(struct sockaddr_in)) < 0) {
        LOG("ERROR connecting to LoadBalancer");
        exit(1);
    }

    loadBalancer.socket = lb_socket;
    loadBalancer.server = lb_addr;

    LOG("Connected to LoadBalancer");
}

void send_request()
{
    connect_to_lb();

    build_req_msg_t req;
    req.platform = p9400_cetus;
    req.listen_port = output_listen_port;

    send_message(loadBalancer.socket, SECRETARY_BUILD_REQ, sizeof(build_req_msg_t), (char*)&req);

    for (;;) {
        int byte_read;
        char buffer[2 * 256] = {0};
        byte_read = read(loadBalancer.socket, buffer, sizeof(buffer));
        if (byte_read > 0) {
            process_message(buffer);
        } else {
            LOG("WARNING: lost connection with load balancer");
            close(loadBalancer.socket);
            break;
        }
    }
}

void process_build_res(build_res_msg_t* message) 
{
    build_res.status = message->status;
    build_res.reason = message->reason;
    if (message->status) {
        LOG("Build finished");
    } else {
        LOG("Build could not start! Reason: %d", message->reason);
        pthread_kill(output_listener.thread_id, SIGTERM);
    }

    close(loadBalancer.socket);
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

void sigint_handler()
{
    pthread_cancel(output_listener.thread_id);
    close(loadBalancer.socket);
    close(output_listener.socket);

    LOG("WARNING Client going down");

    exit(SIGINT);
}

int main()
{
    void *res;
    signal(SIGINT, sigint_handler);
    if (init_log() != 0) {
        perror("WARNING: the log file could not be opened!\n");
        exit(1);
    }

    get_lb_address();

    listen_for_output();
    send_request();

    return 0;
}

