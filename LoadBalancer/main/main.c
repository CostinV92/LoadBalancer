#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <netdb.h>

#include "log.h"
#include "listener.h"
#include "secretary.h"

extern listener_t *listener;

void sigint_handler()
{
	int iSetOption = 1;
	// this is for reusing the port imeddiatly after ctr+c
	setsockopt(listener->socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

	for(int i = 0; i < listener->no_of_secretaries; i++) {
		pthread_cancel(listener->secretaries[i].thread_id);
		close(listener->secretaries[i].client_socket);
	}

	close(listener->socket);
}

void signal_handler(int signum)
{
	switch(signum) {
		case SIGINT:
			sigint_handler();
			break;
	}

	write(1, "\n", 1);
	exit(signum);
}

int main()
{
	signal(SIGINT, signal_handler);
	if(init_log() != 0) {
		printf("WARNING: the log file could not be opened!\n");
	}
	init_client_listener();

	return 0;
}
