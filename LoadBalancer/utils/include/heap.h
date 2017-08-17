#ifndef __HEAP_H__
#define __HEAP_H__

#define MAX_NODES		   	100
#define HEAP_LOAD			(current_index - 1)

typedef struct HEAP_INFO {
	int heap_key;
	int heap_index;
} heap_node_t;

heap_node_t* heap[MAX_NODES];

void heap_push(heap_node_t*);
heap_node_t* heap_pop();
void heap_update_node_key(heap_node_t*, int);

#endif