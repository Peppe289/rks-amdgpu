#ifndef _RKS_DATA_STRUCT_H__
#define _RKS_DATA_STRUCT_H__

typedef struct node_t
{
    void *data;
    struct node_t *next;
} node_t;


typedef struct p_gpu {
    char *root;
    char *hwmon;
} p_gpu;

void push(struct node_t **_node, void *data);
void *pop(struct node_t **_node);
void destroy_node(struct node_t **_node);

const char *get_root(struct node_t *_node);
const char *get_hwmon(struct node_t *_node);
void free_pgpu(struct p_gpu *data);
int pwm_init(struct node_t *_node);
int pwm_control(struct node_t *_node);

#endif // _RKS_DATA_STRUCT_H__