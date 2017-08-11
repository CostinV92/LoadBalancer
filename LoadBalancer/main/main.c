#include <stdlib.h>
#include <signal.h>
#include <netdb.h>

#include "listener.h"

void sigint_handler()
{
	int iSetOption = 1;
	//this is for reusing the port imeddiatly after ctr+c
	setsockopt(listener.socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
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
	init_client_listener();

	return 0;
}
