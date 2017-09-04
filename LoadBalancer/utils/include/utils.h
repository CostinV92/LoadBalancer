#ifndef __LOG_H__
#define __LOG_H__

#define LOG_PATH "/tmp/LoadBalancer.log"

typedef enum BOOL {
	false,
	true
} bool;

typedef enum PLATFORM {
	p9400_cetus
} platform_t;

typedef enum  MESSAGE_TYPE {
	SECRETARY_BUILD_REQ
} message_type_t;

typedef struct MESSAGE {
	message_type_t 			type;
	char					buffer[];
} message_t;


int init_log();
void LOG(char*, ...);
char* format_ip_addr(unsigned long);
void process_message();

#endif