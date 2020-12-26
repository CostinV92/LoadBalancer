#include <stdlib.h>
#include <stdio.h>

#include "liblist.h"

struct list {
    node_t  *head;
    node_t  *tail;
    node_t  *iterator;
};

void list_node_init(node_t *node)
{
    node->next = NULL;
    node->prev = NULL;
}

list_t* list_new()
{
    list_t *new_list = NULL;

    new_list = calloc(1, sizeof(list_t));
    if (!new_list) {
        perror("Error on allocating list.");
    }

    return new_list;
}

void list_delete(list_t **list_p)
{
    list_t *list = NULL;
    node_t *iterator = NULL;
    node_t *working_node = NULL;

    if (!list_p || !(*list_p))
        return;

    list = *list_p;
    iterator = list->head;

    while (iterator) {
        working_node = iterator;
        iterator = iterator->next;

        working_node->next = NULL;
        working_node->prev = NULL;
    }

    free(list);
    *list_p = NULL;
}

void list_add_front(list_t *list, node_t *node)
{
    if (!list || !node)
        return;

    if (list->head)
        list->head->prev = node;

    node->next = list->head;
    list->head = node;

    if (!node->next)
        list->tail = node;
}

void list_add_back(list_t *list, node_t *node)
{
    if (!list || !node)
        return;

    if (list->tail)
        list->tail->next = node;

    node->prev = list->tail;
    list->tail = node;

    if (!node->prev)
        list->head = node;
}

void list_add_after(node_t *node, node_t *new_node)
{
    node_t *next_node = NULL;

    if (!node || !new_node)
        return;

    next_node = node->next;
    if (next_node) {
        new_node->next = next_node;
        next_node->prev = new_node;
    }

    node->next = new_node;
    new_node->prev = node;
}

void list_node_delete(list_t *list, node_t *node)
{
    node_t *next_node = NULL;
    node_t *prev_node = NULL;

    if (!list || !node)
        return;

    if (list->iterator && list->iterator == node)
        list->iterator = node->prev;

    next_node = node->next;
    prev_node = node->prev;

    if (next_node)
        next_node->prev = prev_node;
    else
        list->tail = prev_node;

    if (prev_node)
        prev_node->next = next_node;
    else
        list->head = next_node;

    node->next = NULL;
    node->prev = NULL;
}

node_t* list_node_next(node_t *node)
{
    if (!node)
        return NULL;

    return node->next;
}

node_t* list_node_prev(node_t *node)
{
    if (!node)
        return NULL;

    return node->prev;
}

int list_iterate(list_t *list, node_t **node_p)
{
    if (!list || !node_p)
        return 0;

    if (!list->iterator)
        list->iterator = list->head;
    else
        list->iterator = list->iterator->next;

    *node_p = list->iterator;

    if (*node_p)
        return 1;
    else
        return 0;
}
