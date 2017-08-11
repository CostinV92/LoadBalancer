#ifndef __LISTENER_H__
#define __LISTENER_H__

#include "secretary.h"

#define INCOMING_PORT 				7891
#define MAX_NO_OF_SECRETARIES 		200

typedef struct LISTENER {
	int socket;
	struct sockaddr_in server;

	int no_of_secretaries;
	secretary_t secretaries[MAX_NO_OF_SECRETARIES];
} listener_t;

void init_client_listener();

#endif 
