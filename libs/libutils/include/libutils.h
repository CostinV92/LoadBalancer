#ifndef __LIBUTILS_H__
#define __LIBUTILS_H__

#include <netinet/in.h>

#include "messages.h"

#define MAX_MESSAGE_SIZE 512
#define MAX_IP_ADDR_SIZE  16

#ifdef DEBUG_DISABLE_LOG
#define LOG(...)                LOG_dummy(__VA_ARGS__)
#define utils_init_log(...)     utils_init_log_dummy(__VA_ARGS__)
#define utils_close_log(...)    utils_close_log_dummy(__VA_ARGS__)
#endif /* DEBUG_DISABLE_LOG */

int utils_init_log(char *log_file_path, int path_size);
void utils_close_log();
void LOG(char* format, ...);

void utils_format_ip_addr(struct sockaddr_in* addr, char *output);

int utils_send_message(int socket,
                       message_type_t message_type,
                       int size,
                       char* buffer);
int utils_receive_message_from_socket(int socket, header_t *buffer);

#endif
