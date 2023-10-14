/**
 * Via sysctl manage the fan pwm.
 * This module allows you to fully manage the fan of your AMD GPU.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "data_struct.h"
#include "utils.h"

#define FAN_GPU_FULL    0
#define FAN_GPU_MANU    1
#define FAN_GPU_AUTO    2

const int speed_matrix[6][2] = {
    /* temp - speed percentage */
    {     0,    25},
    {    30,    25},
    {    50,    30},
    {    60,    40},
    {    70,    70},
    {    74,   100},
};

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

static int manual_pwm(struct node_t *_node) {
    FILE *fp;
    char path[255];
    int buff;

    memcpy(path, get_hwmon(_node), strlen(get_hwmon(_node)) + 1);
    strcat(path, "pwm1_enable"); // for now controll only first node

    fp = fopen(path, "r+");
    validate_alloc(fp);
    fscanf(fp, "%d", &buff);

    if (buff == FAN_GPU_MANU) {
        fclose(fp);
        return 0; // already set to manual mode
    }

    buff = fprintf(fp, "%d", FAN_GPU_MANU); // set to manual mode
    fclose(fp);
    return buff;
}

static int get_thermal(struct node_t *_node) {
    FILE *fp;
    char path[255];
    int buff;

    memcpy(path, get_hwmon(_node), strlen(get_hwmon(_node)) + 1);
    strcat(path, "temp1_input"); // for now controll only first node

    fp = fopen(path, "r");
    validate_alloc(fp);
    fscanf(fp, "%d", &buff);
    fclose(fp);
    return buff;
}

/**
 * get range of percentage with thermal.
 * @speed_matrix: info with thermal and speed
*/
static int getIndex_Therm(int therm, int index) {
    if ((index - 1) >= 6)
        return index;

    if (therm > speed_matrix[index][0] &&
        therm < speed_matrix[index + 1][0])
        return index;

    index++;
    return getIndex_Therm(therm, index);
}

/**
 *
 * !
 * |   /
 * |  /
 * | / 
 *  ---------
 * 
 * Through partial parameters I have to obtain
 * the correct temperature to set.
 * 
 * Forget it, it's just math stuff.
 */
static int u_fanspeed(int therm, int index) {
    int x1 = speed_matrix[index][0];
    int x2 = speed_matrix[index + 1][0];
    int y1 = speed_matrix[index][1];
    int y2 = speed_matrix[index + 1][1];
    int x = therm;
    int m = (y2 - y1) / (x2 - x1);
    int q = y1 - m * x1;

    return m * x + q;
}

static void set_speed_matrix(struct node_t *_node) {
    FILE *fp;
    char path[255];
    int data, therm, index;

    memcpy(path, get_hwmon(_node), strlen(get_hwmon(_node)) + 1);
    strcat(path, "pwm1"); // for now controll only first node

    therm = get_thermal(_node); // collect temp information
    therm = therm / 1000;
    index = getIndex_Therm(therm, 0); // get index of matrix for set fan

    // todo -> get exactly number to put.
    data = u_fanspeed(therm, index);
    data = data * 2.55;
    print_info_func("Set %d percentage with %d temp\n", data, therm);

    fp = fopen(path, "w+");
    validate_alloc(fp);
    fprintf(fp, "%d", data);
    fclose(fp);
}

void pwm_control(struct node_t *_node) {

    if (manual_pwm(_node) != 0) {
        print_err_func("Error to set manual mode in pwm\n");
        exit(-1);
    }

    for_each_gpu(_node)
        set_speed_matrix(_node);
}

