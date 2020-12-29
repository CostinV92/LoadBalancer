#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include "libutils.h"

#define MAX_LOG_FILE_PATH_SIZE 200

static FILE *log_file;
static char ip_string[20];

static int utils_send_message_to_socket(int socket, header_t *message);
static int utils_write_to_socket(int socket,
                                 char *buffer,
                                 int bytes_to_write);
static int utils_read_from_socket(int socket,
                                  char *buffer,
                                  int bytes_to_read);


int utils_init_log(char *log_file_path, int path_size) 
{
    char buffer[MAX_LOG_FILE_PATH_SIZE] = {0};
    if (!log_file_path || !path_size || path_size > MAX_LOG_FILE_PATH_SIZE) {
        return -1;
    }

    strncpy(buffer, log_file_path, path_size);
    log_file = fopen(buffer, "a");

    if (!log_file) {
        return -1;
    }

    return 0;
}

void LOG(char* format, ...)
{
    if (log_file) {
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
}

char* utils_format_ip_addr(struct sockaddr_in* addr)
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

int utils_send_message(int socket,
                       message_type_t message_type,
                       int size,
                       char* buffer)
{
    int rc = 0;
    header_t* message = NULL;

    if (!socket || !message_type || !size || !buffer)
        return -1;

    message = calloc(1, sizeof(header_t) + size);
    if (!message) {
        LOG("error: %s() cannot allocate memory.", __FUNCTION__);
        return -1;
    }

    message->type = message_type;
    message->size = size;
    memcpy(message->buffer, buffer, size);

    rc = utils_send_message_to_socket(socket, (header_t *)message);
    free(message);

    return rc;
}

static int utils_send_message_to_socket(int socket, header_t *message)
{
    int rc = 0;
    int message_size = 0;

    if (!socket || !message)
        return -1;

    message_size = sizeof(header_t) + message->size;

    rc = utils_write_to_socket(socket, (char *)message, message_size);
    return rc;
}

static int utils_write_to_socket(int socket,
                                 char *buffer,
                                 int bytes_to_write)
{
    int rc = 0;
    int bytes_written = 0;
    int offset = 0;

    if (!socket || !buffer || !bytes_to_write)
        return -1;

    while ((bytes_written = write(socket,
                                  buffer + offset,
                                  bytes_to_write - offset)) + offset < bytes_to_write) {
        if (bytes_written == -1)
            return -1;
        else if (bytes_written == 0)
            return 1;

        offset += bytes_written;
    }

    return 0;
}

int utils_receive_message_from_socket(int socket, header_t *message)
{
    int rc = 0;
    int header_size = 0;
    int payload_size = 0;

    if (!socket || !message)
        return -1;

    header_size = sizeof(header_t);
    rc = utils_read_from_socket(socket, (char *)message, header_size);
    if (rc != 0)
        return rc;

    payload_size = message->size;
    rc = utils_read_from_socket(socket, ((char *)message) + header_size, payload_size);
    if (rc != 0)
        return rc;
}

static int utils_read_from_socket(int socket,
                                  char *buffer,
                                  int bytes_to_read)
{
    int rc = 0;
    int bytes_read = 0;
    int offset = 0;

    if (!socket || !buffer || !bytes_to_read)
        return -1;

    while ((bytes_read = read(socket,
                              buffer + offset,
                              bytes_to_read - offset)) + offset < bytes_to_read) {
        if (bytes_read == -1)
            return -1;
        else if (bytes_read == 0)
            return 1;

        offset += bytes_read;
    }

    return 0;
}
