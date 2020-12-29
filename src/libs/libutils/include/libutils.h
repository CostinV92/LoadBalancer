#ifndef __LIBUTILS_H__
#define __LIBUTILS_H__

#include <netinet/in.h>

#include "messages.h"

#define MAX_MESSAGE_SIZE 512

int utils_init_log(char *log_file_path, int path_size);
void LOG(char* format, ...);

char* utils_format_ip_addr(struct sockaddr_in* addr);

int utils_send_message(int socket,
                       message_type_t message_type,
                       int size,
                       char* buffer);
int utils_receive_message_from_socket(int socket, header_t *buffer);

#endif
