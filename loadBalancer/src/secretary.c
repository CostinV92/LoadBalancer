#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>

#include "secretary.h"
#include "messages.h"
#include "utils.h"
#include "workerListener.h"

extern heap_t *worker_heap;

static bool send_build_res(client_t*, bool, int);
static bool send_build_order(worker_t*, client_t*, build_req_msg_t*);

static void* listen_work_done(void*);

void* assign_secretary(void* arg)
{
	client_t *client = (client_t*)arg;
	char service[20] = {0};

	if(getnameinfo((struct sockaddr *)&(client->addr), sizeof(client->addr),
		client->hostname, sizeof(client->hostname), service, sizeof(service), 0) == 0) {
		LOG("Secretary: Client introduced himself, hostname: %s, ip: %s",
			client->hostname, format_ip_addr(&(client->addr)));
	} else {
		LOG("WARNING Secretary: Client didn't introduced himself, ip: %s",
			format_ip_addr(&(client->addr)));
	}

	for(;;) {
		int byte_read;
		char buffer[2 * 256] = {0};
		byte_read = read(client->socket, buffer, sizeof(buffer));
		if(byte_read > 0) {
			// TODO: DEBUG
			process_message(client, (message_t*)buffer,
				client->hostname, format_ip_addr(&(client->addr)));
		} else {
			LOG("Secretary: Client closed connection, hostname: %s, ip: %s",
				client->hostname, format_ip_addr(&(client->addr)));

			close(client->socket);
			free(client);
			break;
		}
	}
}

void process_build_req(client_t* client, build_req_msg_t* message)
{
	bool another_one;
	// get a worker from the specific heap
	heap_t *heap = worker_heap;
	worker_t *worker;
	heap_node_t *heap_node;

	// for responding to the client
	bool status = true;
	int reason = 0;

	do {
		another_one = false;

		status = true;
		reason = 0;


		heap_node = heap_pop(heap);
		if (heap_node)
			worker = INFO(heap_node, worker_t);
		else
			worker = 0;

		if (!worker) {
			LOG("WARNING Secretary: Don't have any workers available!");
			status = false;
			reason = 1;
			continue;
		}


		pthread_mutex_lock(&(worker->mutex));
		if(status && worker->no_current_builds >= 2) {
			LOG("WARNING Secretary: Best worker already has 2 or more current builds!");
			status = false;
			reason = 2;
			heap_push(heap, &(worker->heap_node));
		}

		if(status && !worker->alive) {
			LOG("WARNING Secretary: Worker no longer available worker: %s  ip: %s",
					worker->hostname, format_ip_addr(&(worker->addr)));
			status = false;
			reason = 3;

			pthread_mutex_unlock(&(worker->mutex));
			pthread_mutex_destroy(&(worker->mutex));
			free(worker);
			worker = NULL;

			another_one = true;
			continue;
		}

		if(status && !send_build_order(worker, client, message)) {
			status = false;
			reason = 4;
		}

		if(status) {
			//Here the worker would have begin the build so update the worker info and re add it to the heap
			worker->no_current_builds++;
			worker->heap_node.heap_key = worker->no_current_builds;
			heap_push(heap, &(worker->heap_node));

			LOG("Secretary: Worker assigned worker: %s  client: %s",
				worker->hostname, client->hostname);
		}

		pthread_mutex_unlock(&(worker->mutex));

		if (!status && reason == 4) {
			pthread_mutex_destroy(&(worker->mutex));
			free(worker);
			worker = NULL;
		}
	} while (another_one);

	if (!status)
		send_build_res(client, status, reason);
}

void process_build_done(worker_t* worker, build_order_done_msg_t* message)
{
	int status = message->status;
	int reason = message->reason;
	client_t client = message->build_order.client;

	if(!status) {
		LOG("WARNING Secretary: Build failed with reason: %d on hostname: %s, ip: %s, for hostname: %s, ip: %s",
			reason, worker->hostname, format_ip_addr(&(worker->addr)),
				client.hostname, format_ip_addr(&(client.addr)));
	} else {
		LOG("Secretary: Build succeded on hostname: %s, ip: %s, for hostname: %s, ip: %s",
			worker->hostname, format_ip_addr(&(worker->addr)),
				client.hostname, format_ip_addr(&(client.addr)));
	}

	send_build_res(&client, status, reason);
	pthread_mutex_lock(&(worker->mutex));
	worker->no_current_builds--;
	//update the heap key
	heap_update_node_key(worker_heap, &(worker->heap_node), worker->no_current_builds);
	pthread_mutex_unlock(&(worker->mutex));
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

	LOG("Send message: Send message SECRETARY_BUILD_RES to client hostname: %s, ip: %s",
		client->hostname, format_ip_addr(&(client->addr)));
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

	LOG("Send message: Send message WORKER_BUILD_ORDER to worker hostname: %s, ip: %s",
		worker->hostname, format_ip_addr(&(worker->addr)));
	return true;
}