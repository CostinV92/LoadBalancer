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

static FILE *log_file;
static char ip_string[20];

extern void process_build_res(void*);

int init_log() 
{
    log_file = fopen(LOG_PATH, "a");

    if (!log_file) {
        return 1;
    }

    return 0;
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

void process_message(message_t* message)
{
    message_type_t msg_type = message->type;

    switch (msg_type) {
        case SECRETARY_BUILD_RES:
            LOG("Procces message: Got SECRETARY_BUILD_RES message");
            process_build_res((void*)(message->buffer));
            break;

        default:
            LOG("Warnng: %s() unknown message.", __FUNCTION__);
            break;
    }
}

int send_message(int socket, message_type_t type, int size, char* buffer)
{
    int bytes_written = 0;

    message_t* msg = malloc(sizeof(message_t) + size);
    if (!msg) {
        LOG("Error: %s() cannot allocate memory.", __FUNCTION__);
        clean_exit(-1);
    }

    msg->type = type;
    msg->size = sizeof(message_t) + size;
    memcpy(msg->buffer, buffer, size);

    bytes_written = write(socket, msg, msg->size);
    free(msg);

    return bytes_written;
}

void clean_exit(int status)
{
    /* TODO(victor) */
    exit(status);
}