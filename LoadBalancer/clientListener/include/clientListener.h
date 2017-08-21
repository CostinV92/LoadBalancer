#ifndef __CLIENT_LISTENER_H__
#define __CLIENT_LISTENER_H__

#include "secretary.h"

#define CLIENT_PORT 				 7891
#define MAX_NO_OF_SECRETARIES		 200

typedef struct CLIENT_LISTENER {
	int 					socket;
	struct sockaddr_in 		server;

	pthread_t 				thread_id;

	// debug purposes only
	int 					no_of_secretaries;
	secretary_t 			secretaries[MAX_NO_OF_SECRETARIES];
} client_listener_t;

void init_client_listener();

#endif 
