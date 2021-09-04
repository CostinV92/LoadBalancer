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

#define LOG_PATH    "/tmp/load_balancer.log"

void clean_exit(int status)
{
    connections_stop_listening();
    utils_close_log();

#ifdef DEBUG_WORKER_LOAD
    worker_listener_close_monitor();
#endif /* DEBUG_WORKER_LOAD */
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
}

int main()
{
    void *res;

    signal(SIGINT, signal_handler);
    if (utils_init_log(LOG_PATH, strlen(LOG_PATH)) != 0) {
        printf("WARNING: the log file could not be opened!\n");
    }

    client_listener_init();
    worker_listener_init();
    connections_start_listening();

    return 0;
}
