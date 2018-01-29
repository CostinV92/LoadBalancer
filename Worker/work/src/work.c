#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#include <pthread.h>

#include "registration.h"
#include "utils.h"
#include "work.h"

static void* start_build(void* arg);
static int connect_to_client(client_t *client, int client_port);
static void send_build_done(build_order_msg_t *req, int status);

void wait_for_work()
{
    int bytes_read;
    char buffer[2 * 256] = {0};
    for(;;) {
        // wait for messages
        bytes_read = read(loadBalancer->socket, buffer, sizeof(buffer));
        if(bytes_read) {
            LOG("Read %d bytes from LoadBalancer (needed %d)", bytes_read, sizeof(build_order_msg_t));
            process_message((message_t*)buffer);
        } else {
            LOG("Lost connection to LoadBalancer");
            close(loadBalancer->socket);
            break;
        }
    }
}

void process_build_order(build_order_msg_t* message)
{
    pthread_t build_thread_id;
    build_order_msg_t *thread_message = calloc(1, sizeof(build_order_msg_t));
    memcpy(thread_message, message, sizeof(build_order_msg_t));
    // spawn a thread to take care of this
    pthread_create(&build_thread_id, NULL, &start_build, thread_message);
}

void* start_build(void* arg)
{
    int pid, output_socket;
    build_order_msg_t *message = (build_order_msg_t*)arg;
    client_t *client = &(message->client);
    build_req_msg_t *request = &(message->request);

    // first connect to the client
    if((output_socket = connect_to_client(client, request->listen_port)) < 0) {
        // error on connecting to client for sending output
        send_build_done(message, -1);
        return NULL;
    }

    LOG("Starting build for client %s", format_ip_addr(&(client->addr)));

    // spawn a process to do the work and wait for it
    if ((pid = fork()) < 0) {
        // error on forking
        LOG("ERROR forking failed");
        send_build_done(message, -1);
    } else if (pid) {
        // in parent
        int status;
        close(output_socket);

        wait(&status);
        LOG("Build for client %s it's over with status: %d", format_ip_addr(&(client->addr)), status);

        send_build_done(message, status);
    } else {
        // in child
        dup2(output_socket, 1);
        close(output_socket);

        // TODO: here put the "build" script
        execl("/bin/echo", "/bin/echo", "Hello from the other side!", (char*)NULL);
    }
}

void send_build_done(build_order_msg_t *req, int status)
{
    build_order_done_msg_t res;
    res.build_order = *req;
    res.status = status;

    send_message(loadBalancer->socket, WORKER_BUILD_DONE, sizeof(build_order_done_msg_t), (char*)&req);
}

int connect_to_client(client_t *client, int client_port)
{
    int client_socket, port = client_port, iSetOption = 1;
    struct sockaddr_in client_addr = client->addr;

    // create the socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

    if(client_socket < 0) {
        LOG("ERROR opening client output socket");
        return -1;
    }

    client_addr.sin_port = htons(port);

    if (connect(client_socket, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_in)) < 0) {
        LOG("ERROR connecting to client output");
        return -1;
    }

    return client_socket;
}