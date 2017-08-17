#include "heap.h"

#include <stdio.h>

int current_index = 1;

#define HEAP_ROOT				(heap[1])
#define LAST_ELEM				(heap[current_index - 1])
#define HAS_PARENT(x)			(x->heap_index != 1)
#define HAS_FIRST_CHILD(x)		((x->heap_index << 1) < current_index)
#define HAS_SECOND_CHILD(x)		(((x->heap_index << 1) + 1) < current_index)
#define PARENT(x)				(heap[(x->heap_index) >> 1])
#define FIRST_CHILD(x)			(heap[(x->heap_index) << 1])
#define SECOND_CHILD(x)			(heap[((x->heap_index) << 1) + 1])
#define MIN_CHILD(x)			(FIRST_CHILD(x)->heap_key < SECOND_CHILD(x)->heap_key ? FIRST_CHILD(x) : SECOND_CHILD(x))

static void heap_lock();
static void heap_unlock();
static heap_node_t* get_parent(heap_node_t*);
static heap_node_t* get_min_child(heap_node_t*);
static void balance_heap_up_from(heap_node_t*);
static void balance_heap_down_from(heap_node_t*);

void heap_lock()
{
	// TODO take the mutex
}

void heap_unlock()
{
	// TODO release the mutex
}

void heap_push(heap_node_t* node)
{
	if(HEAP_LOAD >= MAX_NODES - 1)
		return;

	heap_lock();
	node->heap_index = current_index;
	heap[current_index++] = node;
	balance_heap_up_from(LAST_ELEM);
	heap_unlock();
}

heap_node_t* heap_pop()
{
	heap_node_t* node;
	heap_lock();
	node = HEAP_ROOT;

	if(!node)
		return 0;

	LAST_ELEM->heap_index = 1;
	HEAP_ROOT = LAST_ELEM;
	current_index--;
	heap[current_index] = 0;
	balance_heap_down_from(HEAP_ROOT);
	heap_unlock();

	return node;
}

void heap_update_node_key(heap_node_t* node, int new_key)
{
	heap_lock();
	int old_key = node->heap_key;
	node->heap_key = new_key;

	if(new_key < old_key)
		balance_heap_up_from(node);
	else if (new_key > old_key)
		balance_heap_down_from(node);
	heap_unlock();
}

void balance_heap_up_from(heap_node_t* node)
{
	if(!node)
		return;

	heap_node_t* parent_node = get_parent(node);
	if(!parent_node)
		return;
	
	while(node->heap_key < parent_node->heap_key) {
		int index = node->heap_index;
		parent_node = PARENT(node);
		node->heap_index = parent_node->heap_index;
		parent_node->heap_index = index;
		heap[node->heap_index] = node;
		heap[parent_node->heap_index] = parent_node;

		parent_node = get_parent(node);
		if(!parent_node)
			return;
	}
}

void balance_heap_down_from(heap_node_t* node)
{
	if(!node)
		return;

	heap_node_t* min_child = get_min_child(node);
	if(!min_child)
		return;

	while(node->heap_key > min_child->heap_key) {
		int index = node->heap_index;
		node->heap_index = min_child->heap_index;
		min_child->heap_index = index;
		heap[node->heap_index] = node;
		heap[min_child->heap_index] = min_child;

		min_child = get_min_child(node);
		if(!min_child)
			return;
	}
}

heap_node_t* get_parent(heap_node_t *node)
{
	if(HAS_PARENT(node))
		return PARENT(node);
	else
		return 0;
}

heap_node_t* get_min_child(heap_node_t *node) 
{
	heap_node_t *first_child, *min_child;
	
	if(HAS_FIRST_CHILD(node))
		first_child = FIRST_CHILD(node);
	else
		return 0;

	if(HAS_SECOND_CHILD(node))
		return MIN_CHILD(node);
	else 
		return first_child;
}





