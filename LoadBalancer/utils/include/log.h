#ifndef __LOG_H__
#define __LOG_H__

#define LOG_PATH "/tmp/LoadBalancer.log"

int init_log();
void LOG(char*, ...);
char* format_ip_addr(unsigned long);

#endif