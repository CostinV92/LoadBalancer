#ifndef __LOG_H__
#define __LOG_H__

#define LOG_PATH "/tmp/client.log"

#include <netinet/in.h>

typedef enum BOOL {
	false,
	true
} bool;

typedef enum PLATFORM {
	p9400_cetus
} platform_t;

typedef struct CLIENT {
    int                     socket;
    struct sockaddr_in      addr;

    char                    hostname[256];
} client_t;

int init_log();
void LOG(char*, ...);
char* format_ip_addr(struct sockaddr_in*);
void process_message();
int send_message();

#endif