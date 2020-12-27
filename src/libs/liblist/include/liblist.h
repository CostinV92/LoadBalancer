#ifndef __LIST_H__
#define __LIST_H__

typedef struct list list_t;

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
list_node_t* list_node_next(list_node_t *node);
list_node_t* list_node_prev(list_node_t *node);
int list_iterate(list_t *list, list_node_t **node_p);

#define info_from_node(node, type) ((type *)((char *)node - (char *)(&(((type *)0)->list_node))))

#endif
