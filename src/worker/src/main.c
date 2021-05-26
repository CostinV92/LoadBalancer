#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "libutils.h"
#include "registration.h"
#include "work.h"

#define LOG_PATH "/tmp/worker.log"

extern load_balancer_server_t *loadBalancer;

/* TODO(victor): check for sockets to client too */
void clean_exit(int status)
{
    if (loadBalancer->socket)
        close(loadBalancer->socket);
    utils_close_log();
    exit(status);
}

void sigint_handler(int signum)
{
    clean_exit(signum);
}

int main()
{
    signal(SIGINT, sigint_handler);
    if (utils_init_log(LOG_PATH, strlen(LOG_PATH)) == -1) {
        printf("Error: %s() the log file could not be opened!\n", __FUNCTION__);
        clean_exit(-1);
    }

    register_worker();
    wait_for_work();

    return 0;
}
