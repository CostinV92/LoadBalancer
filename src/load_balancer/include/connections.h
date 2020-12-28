#ifndef __SECRETARY_H__
#define __SECRETARY_H__

#include "messages.h"

void start_listening();
void connections_unregister_socket(int client_socket);
void connections_process_message(void* peer, header_t* message, char* ip_addr);

int send_message(int socket, message_type_t type, int size, char* buffer);

#endif
