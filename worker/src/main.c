#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"
#include "registration.h"
#include "work.h"

void sigint_handler()
{
    close(loadBalancer->socket);

    LOG("WARNING Worker going down");

    exit(SIGINT);
}

int main()
{
    signal(SIGINT, sigint_handler);
    if(init_log() != 0) {
        printf("WARNING: the log file could not be opened!\n");
        exit(1);
    }

    register_worker();
    wait_for_work();

    return 0;
}