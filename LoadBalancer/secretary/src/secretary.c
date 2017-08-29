#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <netinet/in.h>

#include "secretary.h"
#include "utils.h"

void* assign_secretary(void* arg)
{
	client_t client = *((client_t*)arg);

	for(;;) {
		int byte_read;
		char buffer[256] = {0};
		byte_read = read(client.socket, buffer, sizeof(buffer));
		if(byte_read) {
			// TODO message received --- use a queue or something
			printf("%s\n", buffer);
			process_message((message_t*)buffer);
		} else {
			// TODO connection clossed
			break;
		}
	}
}

void process_build_req(build_req_msg_t message)
{
	// TODO pick a worker... start the build... respond to client
}