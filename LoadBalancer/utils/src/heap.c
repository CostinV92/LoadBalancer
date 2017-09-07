#include "heap.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define HEAP_ROOT(heap)					(((heap_node_t**)heap)[1])
#define LAST_ELEM(heap)					(((heap_node_t**)heap)[heap->current_index - 1])
#define HAS_PARENT(node)				(node->heap_index != 1)
#define HAS_FIRST_CHILD(heap, node)		((node->heap_index << 1) < heap->current_index)
#define HAS_SECOND_CHILD(heap, node)	(((node->heap_index << 1) + 1) < heap->current_index)
#define PARENT(heap, node)				(((heap_node_t**)heap)[(node->heap_index) >> 1])
#define FIRST_CHILD(heap, node)			(((heap_node_t**)heap)[(node->heap_index) << 1])
#define SECOND_CHILD(heap, node)		(((heap_node_t**)heap)[((node->heap_index) << 1) + 1])
#define MIN_CHILD(heap, node)			(FIRST_CHILD(heap, node)->heap_key < SECOND_CHILD(heap, node)->heap_key ? FIRST_CHILD(heap, node) : SECOND_CHILD(heap, node))

static pthread_mutex_t heap_mutex;

static void heap_lock(heap_t*);
static void heap_unlock(heap_t*);
static heap_node_t* get_parent(heap_t*, heap_node_t*);
static heap_node_t* get_min_child(heap_t*, heap_node_t*);
static void balance_heap_up_from(heap_t*, heap_node_t*);
static void balance_heap_down_from(heap_t*, heap_node_t*);

heap_t* heap_init()
{
	// initialize a new heap
	// allocate memory 
	heap_t* new_heap;
	new_heap = (heap_t*)malloc(sizeof(heap_t));

	// init mutex
	pthread_mutex_init(&new_heap->mutex, NULL);

	// init sizes
	new_heap->max_size = MAX_NODES;
	new_heap->current_index = 1;

	return new_heap;
}

static void heap_lock(heap_t* heap)
{
	pthread_mutex_lock(&(heap->mutex));
}

static void heap_unlock(heap_t* heap)
{
	pthread_mutex_unlock(&(heap->mutex));
}

void heap_push(heap_t* heap, heap_node_t* node)
{
	if(heap->current_index >= MAX_NODES)
		return;

	heap_lock(heap);
	node->heap_index = heap->current_index;
	((heap_node_t**)heap)[heap->current_index++] = node;
	balance_heap_up_from(heap, LAST_ELEM(heap));
	heap_unlock(heap);
}

heap_node_t* heap_pop(heap_t* heap)
{
	heap_node_t* node;
	heap_lock(heap);
	node = HEAP_ROOT(heap);

	if(!node)
		return 0;

	LAST_ELEM(heap)->heap_index = 1;
	HEAP_ROOT(heap) = LAST_ELEM(heap);
	heap->current_index--;
	((heap_node_t**)heap)[heap->current_index] = NULL;
	balance_heap_down_from(heap, HEAP_ROOT(heap));
	heap_unlock(heap);

	return node;
}

void heap_update_node_key(heap_t* heap, heap_node_t* node, int new_key)
{
	heap_lock(heap);
	int old_key = node->heap_key;
	node->heap_key = new_key;

	if(new_key < old_key)
		balance_heap_up_from(heap, node);
	else if (new_key > old_key)
		balance_heap_down_from(heap, node);
	heap_unlock(heap);
}

static void balance_heap_up_from(heap_t* heap, heap_node_t* node)
{
	if(!node)
		return;

	heap_node_t* parent_node = get_parent(heap, node);
	if(!parent_node)
		return;
	
	while(node->heap_key < parent_node->heap_key) {
		int index = node->heap_index;
		parent_node = PARENT(heap, node);
		node->heap_index = parent_node->heap_index;
		parent_node->heap_index = index;
		((heap_node_t**)heap)[node->heap_index] = node;
		((heap_node_t**)heap)[parent_node->heap_index] = parent_node;

		parent_node = get_parent(heap, node);
		if(!parent_node)
			return;
	}
}

static void balance_heap_down_from(heap_t* heap, heap_node_t* node)
{
	if(!node)
		return;

	heap_node_t* min_child = get_min_child(heap, node);
	if(!min_child)
		return;

	while(node->heap_key > min_child->heap_key) {
		int index = node->heap_index;
		node->heap_index = min_child->heap_index;
		min_child->heap_index = index;
		((heap_node_t**)heap)[node->heap_index] = node;
		((heap_node_t**)heap)[min_child->heap_index] = min_child;

		min_child = get_min_child(heap, node);
		if(!min_child)
			return;
	}
}

static heap_node_t* get_parent(heap_t* heap, heap_node_t *node)
{
	if(HAS_PARENT(node))
		return PARENT(heap, node);
	else
		return 0;
}

static heap_node_t* get_min_child(heap_t* heap, heap_node_t *node)
{
	heap_node_t *first_child, *min_child;
	
	if(HAS_FIRST_CHILD(heap, node))
		first_child = FIRST_CHILD(heap, node);
	else
		return 0;

	if(HAS_SECOND_CHILD(heap, node))
		return MIN_CHILD(heap, node);
	else 
		return first_child;
}