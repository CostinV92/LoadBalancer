#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

extern void process_build_req(void*);

static FILE *log_file;
static char ip_string[20];

int init_log() 
{
	log_file = fopen(LOG_PATH, "a");

	if(!log_file) {
		return 1;
	}

	return 0;
}

char* format_ip_addr(unsigned long ip)
{
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
	switch(message->type) {
		case SECRETARY_BUILD_REQ:
			process_build_req((void*)(message->buffer));
			break;

		default:
			break;
	}
}