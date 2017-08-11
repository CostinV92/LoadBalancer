#ifndef __SECRETARY_H__
#define __SECRETARY_H__

typedef struct SECRETARY {
	pthread_t thread_id;
	int client_socket;
	struct sockaddr_in client;
} secretary_t;

void* assign_secretary(void*);

#endif