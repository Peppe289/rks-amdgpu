#ifndef _RKS_DATA_STRUCT_H__
#define _RKS_DATA_STRUCT_H__

typedef struct node_t
{
    void *data;
    struct node_t *next;
} node_t;


struct node_t *new_node();
void push(struct node_t **_node, void *data);
void *pop(struct node_t **_node);
void destroy_node(struct node_t **_node);

#endif // _RKS_DATA_STRUCT_H__