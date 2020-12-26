#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"
#include "registration.h"
#include "work.h"

void sigint_handler()
{
    if (loadBalancer->socket)
        close(loadBalancer->socket);
    close_log();

    LOG("Error: worker going down");

    exit(SIGINT);
}

int main()
{
    signal(SIGINT, sigint_handler);
    if (init_log() == -1) {
        printf("Error: %s the log file could not be opened!\n", __FUNCTION__);
        clean_exit();
    }

    register_worker();
    wait_for_work();

    return 0;
}
