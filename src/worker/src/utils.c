#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "utils.h"
#include "messages.h"
#include "work.h"
#include "registration.h"

static FILE *log_file;
static char ip_string[20];

int init_log() 
{
    log_file = fopen(LOG_PATH, "a");

    if (!log_file) {
        return -1;
    }

    return 0;
}

void close_log()
{
    if (log_file)
        fclose(log_file);
}

char* format_ip_addr(struct sockaddr_in* addr)
{
    unsigned long ip = addr->sin_addr.s_addr;
    unsigned short b1, b2, b3, b4;
    ip = ntohl(ip);
    b1 = ip >> 24 & 0xff;
    b2 = ip >> 16 & 0xff;
    b3 = ip >> 8 & 0xff;
    b4 = ip & 0xff;

    snprintf(ip_string, 20, "%d.%d.%d.%d", b1, b2, b3, b4);

    return ip_string;
}

void LOG(char* format, ...)
{
    time_t timestamp;
    struct tm* time_info;

    time(&timestamp);
    time_info = localtime(&timestamp);

    char *date;
    date = asctime(time_info);
    date[strlen(date) - 1] = '\0';

    fprintf(log_file, "[ %s ] ", date);

    va_list arg;
    va_start (arg, format);
    vfprintf(log_file, format, arg);
    va_end(arg);

    fprintf(log_file, "\n");

    fflush(log_file);
    fsync(fileno(log_file));
}

void process_message(header_t* message)
{
    message_type_t msg_type = message->type;

    switch (msg_type) {
        case BUILD_ORDER:
            LOG("got BUILD_ORDER");
            process_build_order((void*)(message->buffer));
            break;

        default:
            LOG("Error: %s() unknown message", __FUNCTION__);
            break;
    }
}

int send_message(int socket, message_type_t type, int size, char* buffer)
{
    int bytes_written = 0;

    header_t* msg = malloc(sizeof(header_t) + size);
    if (!msg) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit();
    }

    msg->type = type;
    msg->size = size;
    memcpy(msg->buffer, buffer, size);

    bytes_written = write(socket, msg, sizeof(header_t) + msg->size);
    free(msg);

    return bytes_written;
}

/* TODO(victor): check for sockets to client too */
void clean_exit()
{
    if (loadBalancer->socket)
        close(loadBalancer->socket);
    close_log();
    exit(-1);
}
