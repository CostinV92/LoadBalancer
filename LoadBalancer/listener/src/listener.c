#include <stdlib.h>
#include <stdio.h>

#include <netdb.h>
#include <netinet/in.h>

#include <ctype.h>

#include <string.h>

#include <pthread.h>

#include "log.h"
#include "secretary.h"

#include "listener.h"

void* start_server(void*);
void create_server();

listener_t *listener;

void init_client_listener()
{
	void *res;
	int socket;

	create_server();

	pthread_t server_thread_id;
	pthread_create(&server_thread_id, NULL, &start_server, &(listener->socket));

	// TODO i think i'll not make this here (global structure wink wink)
	//wait for the thread to join
	pthread_join(server_thread_id, res);
}

void* start_server(void* arg) 
{
	int iSetOption = 1;
	int client_socket, client_len, *socket = (int*)arg;
	struct sockaddr_in client;
	secretary_t secretary;
	pthread_t secretary_thread_id;

	while(1) {
		client_socket = accept(*socket, (struct sockaddr*)&client, &client_len);
		if(client_socket < 0) {
			perror("ERROR on accepting connection");
			//this is for reusing the port imeddiatly after error
			setsockopt(*socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
			exit(1);
		} else {
			LOG("Client connected");

			pthread_create(&secretary_thread_id, NULL, &assign_secretary, &client_socket);

			secretary.thread_id = secretary_thread_id;
			secretary.client_socket = client_socket;
			secretary.client = client;
			listener->secretaries[listener->no_of_secretaries++] = secretary;
		}
	}
}

void create_server() 
{
	int server_socket, port;
	struct sockaddr_in server;

	// create the socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(server_socket < 0) {
		perror("ERROR opening client listener socket");
		exit(1);	
	}

	// init address structure
	memset(&server, 0, sizeof(server));

	port = INCOMING_PORT;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	// bind the socket with the address
	if(bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0 ) {
		perror("ERROR on biding client listener socket");
		exit(1);
	}

	// mark socket as pasive socket
	if(listen(server_socket,20) < 0) {
		perror("ERROR on marking client listener socket as passive");
		exit(1);
	}

	// save listener info
	listener = malloc(sizeof(listener_t));
	listener->socket = server_socket;
	listener->server = server;
	listener->no_of_secretaries = 0;
}
