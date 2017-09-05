#ifndef __LOG_H__
#define __LOG_H__

#define LOG_PATH "/tmp/LoadBalancer.log"

#include <netinet/in.h>

typedef enum BOOL {
	false,
	true
} bool;

typedef enum PLATFORM {
	p9400_cetus
} platform_t;

typedef enum  MESSAGE_TYPE {
	SECRETARY_BUILD_REQ,
	SECRETARY_BUILD_RES,
	WORKER_BUILD_ORDER
} message_type_t;

typedef struct MESSAGE {
	message_type_t 			type;
	int 					size;
	char					buffer[];
} message_t;


int init_log();
void LOG(char*, ...);
char* format_ip_addr(struct sockaddr_in*);
void process_message();
int send_message();

#endif