#ifndef __LIBUTILS_H__
#define __LIBUTILS_H__

#include <netinet/in.h>

#define MAX_MESSAGE_SIZE 512

int utils_init_log(char *log_file_path, int path_size);
void LOG(char* format, ...);

char* utils_format_ip_addr(struct sockaddr_in* addr);

int utils_receive_message_from_socket(int socket, char *buffer);

#endif
