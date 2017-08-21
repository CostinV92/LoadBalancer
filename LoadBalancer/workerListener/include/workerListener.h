#ifndef __WORKER_LISTENER_H__
#define __WORKER_LISTENER_H__

#include "heap.h"

#define WORKER_PORT			7892

typedef enum  MESSAGE_TYPE {
	WORKER_LISTENER_HOSTNAME_REQ,
	WORKER_LISTENER_HOSTNAME_RES
} message_type_t;

typedef struct MESSAGE {
	message_type_t 			message_type;
	int 					message_size;
	char					buffer[];
} message_t;

typedef struct WORKER_LISTENER {
	int 					socket;
	struct sockaddr_in 		server;

	pthread_t 				thread_id;
} worker_listener_t;


typedef struct WORKER {
	// the heap info; must be first in structure
	heap_node_t 			heap_node;

	// network info
	int 					worker_socket;
	struct sockaddr_in 		worker_addr;

	// worker info
	char 					hostname[256];
	int 					no_current_builds;
} worker_t;

void init_worker_listener();

#endif