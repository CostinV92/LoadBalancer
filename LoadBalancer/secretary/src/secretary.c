#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>

#include "secretary.h"
#include "utils.h"
#include "workerListener.h"

void* assign_secretary(void* arg)
{
	client_t client = *((client_t*)arg);
	char service[20] = {0};

	if(getnameinfo((struct sockaddr *)&(client.addr), sizeof(client.addr), client.hostname, sizeof(client.hostname), service, sizeof(service), 0) == 0) {
		LOG("Secretary: Client introduced himself, hostname: %s, ip: %s", client.hostname, format_ip_addr(((struct sockaddr_in*)&(client.addr))->sin_addr.s_addr));
	} else {
		LOG("WARNING Secretary: Client didn't introduced himself, ip: %s", format_ip_addr(((struct sockaddr_in*)&(client.addr))->sin_addr.s_addr));
	}

	for(;;) {
		int byte_read;
		char buffer[256] = {0};
		byte_read = read(client.socket, buffer, sizeof(buffer));
		if(byte_read) {
			// DEBUG
			printf("%s", buffer);
			process_message(&client, (message_t*)buffer);
		} else {
			LOG("Secretary: Client closed connection, hostname: %s, ip: %s", client.hostname, format_ip_addr(((struct sockaddr_in*)&(client.addr))->sin_addr.s_addr));
			break;
		}
	}
}

void process_build_req(client_t* client, build_req_msg_t* message)
{
	// get a worker
	//TODO test if fast build is wanted and pop the specific heap
	worker_t *worker;
	worker = (worker_t*)heap_pop();

	if(worker) {
		if(worker->no_current_builds >= 2) {
			LOG("WARNING Secretary: Best worker already has 2 or more current builds!");
			//TODO change some messages with the client (maybe he will want a fast build)
		}

		// TODO tell the client to listen for output, send to worker the platform and the port for sending the output

		//Here the worker would have begin the build so add it again to the heap (specific heap)
		LOG("Secretary: Worker assigned; worker: %s  client: %s", worker->hostname, client->hostname);
		if(worker->fast_builder) {
			// add to fast builders' heap
		} else {
			heap_push((heap_node_t*)worker);
		}

	} else {
		LOG("WARNING Secretary: Don't have any worker registered!");
		// TODO send this information to client
	}
}