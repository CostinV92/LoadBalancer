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
	client_t		client;
} secretary_t;

typedef struct BUILD_REQ_MSG {
	platform_t 		platform;
	int 			listen_port;
	bool			fast;
} build_req_msg_t;

typedef struct BUILD_RES_MSG {
	bool 			status;
	int 			reason;
}	build_res_msg_t;

typedef struct BUILD_ORDER_MSG {
	client_t			client;
	build_req_msg_t		request;
} build_order_msg_t;

void* assign_secretary(void*);

#endif