#ifndef __LIST_H__
#define __LIST_H__

typedef struct list list_t;
typedef struct list_it list_it;

typedef struct node {
    struct node *next;
    struct node *prev;
} list_node_t;

void list_node_init(list_node_t *node);
list_t* list_new();
void list_delete(list_t **list_p);

void list_add_front(list_t *list, list_node_t *node);
void list_add_back(list_t *list, list_node_t *node);
void list_add_after(list_node_t *node, list_node_t *new_node);
void list_node_delete(list_t *list, list_node_t *node);

list_node_t* list_head(list_t *list);
list_node_t* list_tail(list_t *list);
list_node_t* list_node_next(list_node_t *node);
list_node_t* list_node_prev(list_node_t *node);

list_it* list_begin(list_t *list);
int list_end(list_it *it);
void list_next(list_it **it);
list_node_t *list_node_from_it(list_it *it);

#define list_iterate(list, it)      \
    for(it = list_begin(list);      \
        !list_end(it);              \
        list_next(&it))             \

#define info_from_node(node, node_name, type)   ((type *)((char *)node - (char *)(&(((type *)0)->node_name))))
#define info_from_it(it, node_name, type)       (info_from_node(list_node_from_it(it), node_name, type))

#endif
