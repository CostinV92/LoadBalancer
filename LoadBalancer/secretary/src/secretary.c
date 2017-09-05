#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>

#include "secretary.h"
#include "utils.h"
#include "workerListener.h"

extern heap_t *worker_heap, *fast_worker_heap;

static bool send_build_res(client_t*, bool, int);
static bool send_build_order(worker_t*, client_t*, build_req_msg_t*);

void* assign_secretary(void* arg)
{
	client_t client = *((client_t*)arg);
	char service[20] = {0};

	if(getnameinfo((struct sockaddr *)&(client.addr), sizeof(client.addr), client.hostname, sizeof(client.hostname), service, sizeof(service), 0) == 0) {
		LOG("Secretary: Client introduced himself, hostname: %s, ip: %s", client.hostname, format_ip_addr(&(client.addr)));
	} else {
		LOG("WARNING Secretary: Client didn't introduced himself, ip: %s", format_ip_addr(&(client.addr)));
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
			LOG("Secretary: Client closed connection, hostname: %s, ip: %s", client.hostname, format_ip_addr(&(client.addr)));
			break;
		}
	}
}

void process_build_req(client_t* client, build_req_msg_t* message)
{
	bool another_one;
	// get a worker from the specific heap
	heap_t *heap = message->fast ? fast_worker_heap : worker_heap;
	worker_t *worker;

	// for responding to the client
	bool status = true;
	int reason = 0;

	do {
		another_one = false;

		status = true;
		reason = 0;

		worker = (worker_t*)heap_pop(heap);

		if(worker) {
			if(worker->no_current_builds >= 2) {
				LOG("WARNING Secretary: Best worker already has 2 or more current builds!");
				status = false;
				reason = 1;
			}
		} else {
			LOG("WARNING Secretary: Don't have any worker registered!");
			status = false;
			reason = 2;
		}

		if(send_build_res(client, status, reason)) {
			//send build and client info to the worker
			if(send_build_order(worker, client, message)) {
				//Here the worker would have begin the build so update the worker info and re add it to the heap
				worker->no_current_builds++;
				worker->heap_node.heap_key = worker->no_current_builds;
				heap_push(heap, (heap_node_t*)worker);

				LOG("Secretary: Worker assigned worker: %s  client: %s", worker->hostname, client->hostname);
			} else {
				// the worker disconnected
				// free the memory for the disconnected worker
				LOG("WARNING Secretary: Worker no longer available worker: %s  ip: %s", worker->hostname, format_ip_addr(&(worker->addr)));
				free(worker);
				worker = NULL;

				// pick another worker
				another_one = true;
			}
		} else {
			// the build couldn't be started so add the worker back to the heap
			heap_push(heap, (heap_node_t*)worker);
		}
	} while (another_one);
}

static bool send_build_res(client_t* client, bool status, int reason)
{
	build_res_msg_t res_msg;

	res_msg.status = status;
	res_msg.reason = reason;

	if (send_message(client->socket, SECRETARY_BUILD_RES, sizeof(build_res_msg_t), (char*)&res_msg) < 0) {
		//here we will return, the unconnected client will be logged by the read in assign secretary
		return false;
	}

	LOG("Send message: Send message SECRETARY_BUILD_RES to client hostname: %s, ip: %s", client->hostname, format_ip_addr(&(client->addr)));
	return true;
}

static bool send_build_order(worker_t* worker, client_t* client, build_req_msg_t* build_message)
{
	build_order_msg_t build_order;

	build_order.client = *client;
	build_order.request = *build_message;

	if(send_message(worker->socket, WORKER_BUILD_ORDER, sizeof(build_order_msg_t), (char*)&build_order) < 0) {
		// return, the unconnected worker will be handled in process_build_req
		return false;
	}

	LOG("Send message: Send message WORKER_BUILD_ORDER to worker hostname: %s, ip: %s", worker->hostname, format_ip_addr(&(worker->addr)));
	return true;
}