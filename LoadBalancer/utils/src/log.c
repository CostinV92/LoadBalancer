#include <stdio.h>

#include "log.h"

static FILE *log_file;

int init_log() 
{
	log_file = fopen(LOG_PATH, "a");

	if(!log_file) {
		return 1;
	}

	return 0;
}

void LOG(char* buffer)
{
	fprintf(log_file, "%s\n", buffer);
}