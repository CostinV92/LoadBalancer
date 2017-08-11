#ifndef __LISTENER_H__
#define __LISTENER_H__

#define INCOMING_PORT 6969

struct LISTENER {
	int socket;
	struct sockaddr_in server;
} listener;

void init_client_listener();

#endif 
