#include <stdlib.h>
#include <stdio.h>

#include "liblist.h"

struct list {
    list_node_t  *head;
    list_node_t  *tail;

    struct list_it {
        list_node_t  *next;
        list_node_t  *prev;
        list_node_t  *current;
    } it;
};

void list_node_init(list_node_t *node)
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
    list_node_t *iterator = NULL;
    list_node_t *working_node = NULL;

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

void list_add_front(list_t *list, list_node_t *node)
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

void list_add_back(list_t *list, list_node_t *node)
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

void list_add_after(list_node_t *node, list_node_t *new_node)
{
    list_node_t *next_node = NULL;

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

void list_node_delete(list_t *list, list_node_t *node)
{
    list_node_t *next_node = NULL;
    list_node_t *prev_node = NULL;

    if (!list || !node)
        return;

    next_node = node->next;
    prev_node = node->prev;

    if (next_node)
        next_node->prev = prev_node;
    else if (node == list->tail)
        list->tail = prev_node;

    if (prev_node)
        prev_node->next = next_node;
    else if (node == list->head)
        list->head = next_node;

    if (list->it.next && list->it.next == node)
        list->it.next = list->it.next->next;

    node->next = NULL;
    node->prev = NULL;
}

list_node_t* list_head(list_t *list)
{
    if (!list)
        return NULL;

    return list->head;
}

list_node_t* list_tail(list_t *list)
{
    if (!list)
        return NULL;

    return list->tail;
}

list_node_t* list_node_next(list_node_t *node)
{
    if (!node)
        return NULL;

    return node->next;
}

list_node_t* list_node_prev(list_node_t *node)
{
    if (!node)
        return NULL;

    return node->prev;
}

list_it* list_begin(list_t *list)
{
    if (!list)
        return NULL;

    list->it.next = NULL;
    list->it.current = NULL;

    if (list->head)
        list->it.next = list->head->next;
    list->it.prev = NULL;
    list->it.current = list->head;

    if (list->it.current)
        return &list->it;
    else
        return NULL;
}

int list_end(list_it *it)
{
    if (!it)
        return 1;

    return 0;
}

void list_next(list_it **it)
{
    if (!it || !(*it))
        return;

    (*it)->current = (*it)->next;
    if ((*it)->next)
        (*it)->next = (*it)->next->next;
    else
        *it = NULL;
}

list_node_t *list_node_from_it(list_it *it)
{
    if (!it)
        return NULL;

    return it->current;
}
