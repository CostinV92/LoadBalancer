#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <netinet/in.h>

#include "secretary.h"

void* assign_secretary(void* arg)
{
	int client_socket = *((int*)arg);

	for(;;) {
		int byte_read;
		char buffer[256] = {0};
		byte_read = read(client_socket, buffer, sizeof(buffer));
		if(byte_read) {
			// TODO message received --- use a queue or something
			printf("%s\n", buffer);
		} else {
			// TODO connection clossed
			break;
		}
	}
}