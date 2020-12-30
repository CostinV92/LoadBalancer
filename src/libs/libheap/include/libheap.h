#ifndef __HEAP_H__
#define __HEAP_H__

#include <pthread.h>

#define MAX_NODES           101

typedef struct heap_node {
    int heap_key;
    int heap_index;
} heap_node_t;

typedef struct heap {
    heap_node_t*        heap[MAX_NODES];
    pthread_mutex_t     mutex;
    int                 max_size;
    int                 current_index;
} heap_t;

heap_t* heap_init();
void heap_destroy(heap_t **heap_p);

void heap_push(heap_t*, heap_node_t*);
heap_node_t* heap_pop(heap_t*);
void heap_update_node_key(heap_t*, heap_node_t*, int);

/* TODO(victor) add node name ;)*/
#define INFO(node, type)            ((type*)((char*)node - (char*)(&(((type*)0)->heap_node))))

#endif
