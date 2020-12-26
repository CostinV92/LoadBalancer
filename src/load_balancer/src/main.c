#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <netdb.h>

#include "utils.h"
#include "clientListener.h"
#include "workerListener.h"
#include "secretary.h"

extern client_listener_t *client_listener;
extern worker_listener_t *worker_listener;

void sigint_handler()
{
    for (int i = 0; i < client_listener->no_of_secretaries; i++) {
        pthread_cancel(client_listener->secretaries[i].thread_id);
        close(client_listener->secretaries[i].client->socket);
    }

    pthread_cancel(client_listener->thread_id);
    pthread_cancel(worker_listener->thread_id);
    close(client_listener->socket);
    close(worker_listener->socket);
}

void signal_handler(int signum)
{
    switch (signum) {
        case SIGINT:
            sigint_handler();
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
    if (init_log() != 0) {
        printf("WARNING: the log file could not be opened!\n");
    }
    init_client_listener();
    init_worker_listener();

    // the thread should never join as the server must run constantly
    pthread_join(client_listener->thread_id, res);

    return 0;
}
