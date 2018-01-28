#include <unistd.h>

#include "registration.h"
#include "work.h"
#include "utils.h"

extern load_balancer_server_t *loadBalancer;

void wait_for_work()
{
    int bytes_read;
    char buffer[256] = {0};
    for(;;) {
        // wait for messages
        bytes_read = read(loadBalancer->socket, buffer, sizeof(buffer));
        if(bytes_read) {
            //process_message(client, (message_t*)buffer);
        } else {
            LOG("Lost connection to LoadBalancer");
            break;
        }
    }
}