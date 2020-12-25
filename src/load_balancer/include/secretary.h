#ifndef __SECRETARY_H__
#define __SECRETARY_H__

#include "utils.h"

typedef struct CLIENT {
	int 					socket;
	struct sockaddr_in		addr;

	char 					hostname[256];
} client_t;

typedef struct SECRETARY {
	pthread_t		thread_id;
	client_t		*client;
} secretary_t;

void* assign_secretary(void*);

#endif