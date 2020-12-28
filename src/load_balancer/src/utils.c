#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <pthread.h>

#include "utils.h"
#include "connections.h"
#include "messages.h"
#include "client_listener.h"
#include "worker_listener.h"

extern client_listener_t *client_listener;
extern worker_listener_t *worker_listener;

extern void process_build_done(client_t*, void*);

void process_message(void* peer, header_t* message, char* ip_addr)
{
    message_type_t msg_type = message->type;

    switch (msg_type) {
        case SECRETARY_BUILD_REQ:
            LOG("client_listener: from %s got SECRETARY_BUILD_REQ", ip_addr);
            process_build_req(peer, (void*)(message->buffer));
            break;

        case WORKER_BUILD_DONE:
            LOG("worker_listener: from %s got WORKER_BUILD_DONE", ip_addr);
            process_build_done(peer, (void*)(message->buffer));
            break;

        default:
            LOG("WARNING Procces message: Unknown message from  ip: %s", ip_addr);
            break;
    }
}

int send_message(int socket, message_type_t type, int size, char* buffer)
{
    int bytes_written = 0;

    header_t* msg = calloc(1, sizeof(header_t) + size);
    if (!msg) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    msg->type = type;
    msg->size = sizeof(header_t) + size;
    memcpy(msg->buffer, buffer, size);

    bytes_written = write(socket, msg, msg->size);
    free(msg);

    return bytes_written;
}

void clean_exit(int status)
{
    pthread_cancel(client_listener->thread_id);
    pthread_cancel(worker_listener->thread_id);
    close(client_listener->socket);
    close(worker_listener->socket);

    exit(status);
}
