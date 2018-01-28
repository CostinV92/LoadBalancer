#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "registration.h"
#include "work.h"
#include "utils.h"

void sigint_handler()
{
    int iSetOption = 1;
    // this is for reusing the port imeddiatly after ctr+c
    setsockopt(loadBalancer->socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

    close(loadBalancer->socket);

    LOG("WARNING Worker going down");

    exit(SIGINT);
}

int main()
{
    signal(SIGINT, sigint_handler);
    if(init_log() != 0) {
        printf("WARNING: the log file could not be opened!\n");
    }

    register_worker();
    wait_for_work();

    return 0;
}