#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <netinet/in.h>

#include "secretary.h"

void* assign_secretary(void* arg)
{
	char buffer[256];
	int client_socket = *((int*)arg);

	// testing purposes only
	write(client_socket, "Hello! I'm youre designated secretary!\n", sizeof("Hello! I'm youre designated secretary!\n"));

	while(1) {
		int bytes_read;
		memset(&buffer, 0, 256);
		bytes_read = read(client_socket, buffer, 256);
		if(bytes_read)
			printf("%s", buffer);
		else
			break;
	}

	// here the connection would be closed by the client
	// end of testing purposes only
}