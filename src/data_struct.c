#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "data_struct.h"

struct node_t *new_node()
{
    struct node_t *ret;
    ret = malloc(sizeof(struct node_t));
    ret->data = NULL;
    ret->next = NULL;
    return ret;
}

void push(struct node_t **_node, void *data)
{
    struct node_t *new_node;

    new_node = malloc(sizeof(struct node_t));
    validate_alloc(new_node);

    new_node->data = data;
    new_node->next = *_node;
    *_node = new_node;
}

void *pop(struct node_t **_node)
{
    void *retval = NULL;
    node_t *next_node = NULL;

    if (*_node == NULL)
    {
        return NULL;
    }

    next_node = (*_node)->next;
    retval = (*_node)->data;
    free(*_node);
    *_node = next_node;

    return retval;
}

void destroy_node(struct node_t **_node)
{
    struct node_t *current = *_node;
    struct node_t *next;

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