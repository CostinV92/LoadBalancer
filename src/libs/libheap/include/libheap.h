#ifndef __HEAP_H__
#define __HEAP_H__

#include <pthread.h>

#define MAX_NODES           101

typedef struct heap heap_t;

typedef struct heap_node {
    int heap_key;
    int heap_index;
} heap_node_t;

heap_t* heap_init();
void heap_destroy(heap_t **heap_p);

void heap_push(heap_t*, heap_node_t*);
heap_node_t* heap_pop(heap_t*);
void heap_update_node_key(heap_t*, heap_node_t*, int);

#define heap_info_from_node(node, node_name, type)      ((type *)((char *)node - \
                                                         (char *)(&(((type *)0)->node_name))))

#endif
