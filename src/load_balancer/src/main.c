#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "libutils.h"
#include "connections.h"
#include "client_listener.h"
#include "worker_listener.h"

extern client_listener_t *client_listener;
extern worker_listener_t *worker_listener;

#define LOG_PATH "/tmp/loadBalancer.log"

void clean_exit(int status)
{
    close(client_listener->socket);
    close(worker_listener->socket);

    exit(status);
}

void signal_handler(int signum)
{
    switch (signum) {
        case SIGINT:
            clean_exit(signum);
            break;
        default:
            break;
    }

    write(1, "\n", 1);
    exit(signum);
}

int main()
{
    void *res;

    signal(SIGINT, signal_handler);
    if (utils_init_log(LOG_PATH, strlen(LOG_PATH)) != 0) {
        printf("WARNING: the log file could not be opened!\n");
    }

    init_client_listener();
    init_worker_listener();
    start_listening();

    return 0;
}
