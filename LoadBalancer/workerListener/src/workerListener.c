#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <ctype.h>

#include <string.h>

#include <pthread.h>

#include "utils.h"

#include "workerListener.h"

static void create_server();
static void* start_server(void*);
static void* register_worker(void*);
static bool get_worker_hostname(worker_t*);
static void listen_to_worker(worker_t*);

worker_listener_t *worker_listener;

void init_worker_listener()
{
	int socket;
	worker_listener = malloc(sizeof(worker_listener_t));

	create_server();

	pthread_t server_thread_id;
	pthread_create(&server_thread_id, NULL, &start_server, &(worker_listener->socket));

	worker_listener->thread_id = server_thread_id;
}

void* start_server(void* arg) 
{
	int iSetOption = 1;
	int worker_socket, worker_len = sizeof(struct sockaddr_in), *socket = (int*)arg;
	struct sockaddr_in worker_addr;

	while(1) {
		worker_socket = accept(*socket, (struct sockaddr*)&worker_addr, &worker_len);
		if(worker_socket < 0) {
			perror("ERROR on accepting worker connection");
			//this is for reusing the port imeddiatly after error
			setsockopt(*socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
			exit(1);
		} else {
			pthread_t worker_thread_id;
			LOG("Worker listener: worker connected, ip: %s", format_ip_addr(((struct sockaddr_in*)&worker_addr)->sin_addr.s_addr));

			worker_t worker;
			memset(&worker, 0, sizeof(worker_t));

			worker.socket = worker_socket;
			worker.addr = worker_addr;
			pthread_create(&worker_thread_id, NULL, &register_worker, &worker);
		}
	}
}

void create_server()
{
	int server_socket, port;
	struct sockaddr_in server_addr;

	// create the socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(server_socket < 0) {
		perror("ERROR opening worker listener socket");
		exit(1);	
	}

	// init address structure
	memset(&server_addr, 0, sizeof(server_addr));

	port = WORKER_PORT;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	// bind the socket with the address
	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
		perror("ERROR on biding worker listener socket");
		exit(1);
	}

	// mark socket as pasive socket
	if(listen(server_socket,20) < 0) {
		perror("ERROR on marking worker listener socket as passive");
		exit(1);
	}

	// save listener info
	worker_listener->socket = server_socket;
	worker_listener->server = server_addr;
}

void* register_worker(void* arg)
{
	char service[20] = {0};
	worker_t *worker = malloc(sizeof(worker_t));
	memset(worker, 0, sizeof(worker_t));
	memcpy((void*)worker, arg, sizeof(worker_t));

	if(getnameinfo((struct sockaddr *)&(worker->addr), sizeof(worker->addr), worker->hostname, sizeof(worker->hostname), service, sizeof(service), 0) == 0) {
		heap_push((heap_node_t*)worker);
		LOG("Worker listener: worker added to database, hostname: %s, ip: %s", worker->hostname, format_ip_addr(((struct sockaddr_in*)&(worker->addr))->sin_addr.s_addr));
	} else {
		LOG("Worker listener: failed to add worker to database (no hostname), ip: %s", format_ip_addr(((struct sockaddr_in*)&(worker->addr))->sin_addr.s_addr));
	}
}