#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "data_struct.h"

void push(struct node_t **_node, void *data)
{
    struct node_t *new_node = malloc(sizeof(struct node_t));
    new_node->data = data;
    new_node->next = NULL;

    if (*_node == NULL)
    {
        *_node = new_node;
    }
    else
    {
        struct node_t *cur = *_node;

        while (cur->next != NULL)
            cur = cur->next;

        cur->next = new_node;
    }
}

void *pop(struct node_t **_node)
{
    void *retval = NULL;
    struct node_t *next_node = NULL;

    if (*_node == NULL)
        return NULL;

    next_node = (*_node)->next;
    retval = (*_node)->data;
    free(*_node);
    *_node = next_node;

    return retval;
}

void destroy_node(struct node_t **_node)
{
    struct node_t *next, *current = *_node;

    while (current != NULL)
    {
        next = current->next;

        if (current->data != NULL)
            free_pgpu(current->data);

        free(current);
        current = next;
    }

    *_node = NULL;
}