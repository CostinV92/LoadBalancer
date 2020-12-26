#ifndef __LOG_H__
#define __LOG_H__

#define LOG_PATH "/tmp/loadBalancer.log"

#include <netinet/in.h>

typedef enum BOOL {
    false = 0,
    true = 1
} bool;

typedef enum PLATFORM {
    p9400_cetus
} platform_t;

int init_log();
void LOG(char*, ...);
char* format_ip_addr(struct sockaddr_in*);
void process_message();
int send_message();

#endif
