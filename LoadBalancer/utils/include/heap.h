#ifndef __HEAP_H__
#define __HEAP_H__

#include <pthread.h>

#define MAX_NODES		   	101

typedef struct HEAP_INFO {
	int heap_key;
	int heap_index;
} heap_node_t;

typedef struct HEAP {
	heap_node_t*		heap[MAX_NODES];
	pthread_mutex_t		mutex;
	int 				max_size;
	int 				current_index;
} heap_t;

heap_t* heap_init();

// define as a macro so it will NULL the pointer after
#define heap_destroy(heap) {free(heap); heap = NULL;}

void heap_push(heap_t*, heap_node_t*);
heap_node_t* heap_pop(heap_t*);
void heap_update_node_key(heap_t*, heap_node_t*, int);

#endif