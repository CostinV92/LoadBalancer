#include <stdio.h>
#include <string.h>

#include "secretary.h"

void* assign_secretary(void* arg)
{
	char buffer[256] = {0};
	int client_socket = *((int*)arg);

	write(client_socket, "Hello! I'm youre designated secretary!\n", sizeof("Hello! I'm youre designated secretary!\n"));

	while(1) {
		bzero(buffer, 256);
		read(client_socket, buffer, 256);
		printf("%s", buffer);
	}
}