#ifndef __SECRETARY_H__
#define __SECRETARY_H__

#include "messages.h"

void connections_start_listening();
void connections_stop_listening();
void connections_unregister_socket(int client_socket);
void connections_process_message(void* peer, header_t* message, char* ip_addr);

#endif
