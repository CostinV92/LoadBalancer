#ifndef __WORKER_LISTENER_H__
#define __WORKER_LISTENER_H__

#include "heap.h"

#define WORKER_PORT			7892

typedef struct WORKER_LISTENER {
	int 					socket;
	struct sockaddr_in 		server;

	pthread_t 				thread_id;
} worker_listener_t;


typedef struct WORKER {
	heap_node_t 			heap_node;

	// network info
	int 					socket;
	struct sockaddr_in 		addr;

	// worker info
	char 					hostname[256];
	int 					no_current_builds;
	pthread_mutex_t 		mutex;
	bool					alive;
} worker_t;

void init_worker_listener();

#endif