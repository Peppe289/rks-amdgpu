/**
 * Via sysctl manage the fan pwm.
 * This module allows you to fully manage the fan of your AMD GPU.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "data_struct.h"
#include "utils.h"

void free_pgpu(struct p_gpu *data) {

    if (data->hwmon != NULL)
        free(data->hwmon);
    
    if (data->root != NULL)
        free(data->root);
    
    free(data);
}

const char *get_root(struct node_t *_node) {
    struct p_gpu *data = _node->data;
    return data->root;
}

const char *get_hwmon(struct node_t *_node) {
    struct p_gpu *data = _node->data;
    return data->hwmon;
}

