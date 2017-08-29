#ifndef __SECRETARY_H__
#define __SECRETARY_H__

#include "utils.h"

typedef struct BUILD_REQ_MSG {
	platform_t 		platform;
	bool			fast;
} build_req_msg_t;

typedef struct CLIENT {
	int 					socket;
	struct sockaddr_in		addr;
} client_t;

typedef struct SECRETARY {
	pthread_t		thread_id;
	client_t		client;
} secretary_t;

void* assign_secretary(void*);

#endif