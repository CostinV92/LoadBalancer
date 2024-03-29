#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#include <pthread.h>

#include "registration.h"
#include "libutils.h"
#include "work.h"

static void* start_build(void* arg);
static int connect_to_client(struct sockaddr_in client_addr, int client_port);
static void send_build_done(build_order_msg_t *req, int status, int reason);
static void process_message(header_t* message);

extern void clean_exit();

extern load_balancer_server_t *loadBalancer;

void wait_for_work()
{
    int rc = 0;
    char buffer[MAX_MESSAGE_SIZE] = {0};
    for (;;) {
        // wait for messages
        rc = utils_receive_message_from_socket(loadBalancer->socket, (header_t *)buffer);
        if (rc != 0) {
            LOG("error: %s() error on reading from socket.", __FUNCTION__);
            close(loadBalancer->socket);
            break;
        }

        process_message((header_t*)buffer);
    }
}

void process_build_order(build_order_msg_t* message)
{
    pthread_t build_thread_id = 0;
    build_order_msg_t *thread_message = NULL;

    thread_message = calloc(1, sizeof(build_order_msg_t));
    if (!thread_message) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit();
    }

    memcpy(thread_message, message, sizeof(build_order_msg_t));
    pthread_create(&build_thread_id, NULL, &start_build, thread_message);
    pthread_detach(build_thread_id);
}

void* start_build(void* arg)
{
    int pid = 0, output_socket = 0;
    build_order_msg_t message = *(build_order_msg_t*)arg;
    struct sockaddr_in client_addr = message.client_addr;
    build_req_msg_t *request = &(message.request);
    char client_ip_addr[MAX_IP_ADDR_SIZE];

    free(arg);

    // first connect to the client
    if ((output_socket = connect_to_client(client_addr, request->listen_port)) == -1) {
        // error on connecting to client for sending output
        send_build_done(&message, 0, 5);
        return NULL;
    }

    utils_format_ip_addr(&client_addr, client_ip_addr);

    LOG("Starting build for client %s", client_ip_addr);

    // spawn a process to do the work and wait for it
    if ((pid = fork()) == -1) {
        LOG("Error:%s forking failed.", __FUNCTION__);
        send_build_done(&message, 0, 6);
    } else if (pid) {
        int status = 0;
        close(output_socket);

        waitpid(pid, &status, 0);

        LOG("Build for client %s it's over with status: %d",
            client_ip_addr, status);

        send_build_done(&message, 1, status);
    } else {
        dup2(output_socket, 1);
        close(output_socket);

        execl("/vagrant/worker/work.sh", "work_lb", (char*)NULL);
    }
}

void send_build_done(build_order_msg_t *req, int status, int reason)
{
    build_order_done_msg_t res;
    res.build_order = *req;
    res.status = status;
    res.reason = reason;

    utils_send_message(loadBalancer->socket,
                       BUILD_DONE,
                       sizeof(build_order_done_msg_t),
                       (char*)&res);
}

int connect_to_client(struct sockaddr_in client_addr, int client_port)
{
    int client_socket = 0, port = client_port, iSetOption = 1;

    // create the socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        LOG("Error: %s() cannot open client output socket.", __FUNCTION__);
        return -1;
    }
    setsockopt(client_socket,
               SOL_SOCKET,
               SO_REUSEADDR,
               (char*)&iSetOption,
               sizeof(iSetOption));

    client_addr.sin_port = htons(port);
    if (connect(client_socket,
                (struct sockaddr *)&client_addr,
                sizeof(struct sockaddr_in)) == -1) {
        LOG("Error: %s() cannot connect to client.", __FUNCTION__);
        return -1;
    }

    return client_socket;
}

static void process_message(header_t* message)
{
    message_type_t msg_type = message->type;

    switch (msg_type) {
        case BUILD_ORDER:
            LOG("got BUILD_ORDER");
            process_build_order((void*)(message->buffer));
            break;

        default:
            LOG("Error: %s() unknown message", __FUNCTION__);
            break;
    }
}
