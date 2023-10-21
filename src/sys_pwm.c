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

const int speed_matrix[][2] = {
    /* temp - speed percentage */
    {     0,    25},
    {    30,    25},
    {    50,    30},
    {    60,    40},
    {    70,    70},
    {    74,   100},
};

const int fan_steps = sizeof(speed_matrix) / sizeof(speed_matrix[0]);

void free_pgpu(struct p_gpu *data) {

    if (data->hwmon != NULL)
        free(data->hwmon);
    
    if (data->root != NULL)
        free(data->root);
    
    free(data);
}

const char *get_root(struct node_t *_node) {
    struct p_gpu *data = _node->data;

    if (data != NULL)
        return data->root;

    return NULL;
}

const char *get_hwmon(struct node_t *_node) {
    struct p_gpu *data = _node->data;

    if (data != NULL)
        return data->hwmon;

    return NULL;
}

static int manual_pwm(struct node_t *_node) {
    int fd, ret;
    char path[PATH_MAX];
    char buffer[10] = {0};
    const char *hwmon = get_hwmon(_node);

    if (hwmon == NULL)
        return -1;

    memcpy(path, hwmon, strlen(hwmon) + 1);
    strcat(path, "pwm1_enable"); // for now controll only first node

    if ((fd = open(path, O_RDWR)) < 0) {
        errno_printf(1, "Error to open %s", path);
        return -1;
    }

    if ((ret = read(fd, buffer, sizeof(buffer))) < 0) {
        errno_printf(1, "Error to read %s", path);
        return -1;
    }

    buffer[ret] = '\0';
    sscanf(buffer, "%d", &ret);
    if (ret == FAN_GPU_MANU) {
        close(fd);
        return 0; // already set to manual mode
    }

    memset(buffer, 0, sizeof(buffer)); // clear buffer
    sprintf(buffer, "%d", FAN_GPU_MANU); // set to manual mode
    if (write(fd, buffer, sizeof(buffer)) < 0) {
        errno_printf(1, "Error to set %s", path);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

static int get_thermal(struct node_t *_node) {
    int fd, ret;
    char path[PATH_MAX];
    char buffer[10];

    memcpy(path, get_hwmon(_node), strlen(get_hwmon(_node)) + 1);
    strcat(path, "temp1_input"); // for now controll only first node

    if ((fd = open(path, O_RDONLY)) < 0) {
        errno_printf(1, "Error to open %s", path);
        return -1;
    }

    if ((ret = read(fd, buffer, sizeof(buffer))) < 0) {
        errno_printf(1, "Error to read %s", path);
        close(fd);
        return -1;
    }
    buffer[ret] = '\0';
    close(fd);
    sscanf(buffer, "%d", &ret);
    return ret;
}

/**
 * get range of percentage with thermal.
 * @speed_matrix: info with thermal and speed
*/
static int getIndex_Therm(int therm)
{
    for (int index = 0; index < fan_steps - 1; index++)
    {
        if (therm > speed_matrix[index][0] && therm <= speed_matrix[index + 1][0])
            return index;
    }

    return fan_steps - 1;
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

static int set_speed_matrix(struct node_t *_node) {
    int fd;
    char path[PATH_MAX];
    char buff[10] = {0};
    int data, therm, index;

    memcpy(path, get_hwmon(_node), strlen(get_hwmon(_node)) + 1);
    strcat(path, "pwm1"); // for now controll only first node

    if ((therm = get_thermal(_node)) < 0) {
        errno_printf(1, "Error to get thermal info"); // collect temp information
        return -1;
    }

    therm = therm / 1000;
    index = getIndex_Therm(therm); // get index of matrix for set fan

    // todo -> get exactly number to put.
    data = u_fanspeed(therm, index);
    data = data * 2.55;
    info_printf("Set %d percentage with %d temp\n", data, therm);

    if ((fd = open(path, O_WRONLY)) < 0) {
        return -1;
    }

    sprintf(buff, "%d", data);
    if (write(fd, buff, strlen(buff) + 1) < 0) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int pwm_init(struct node_t *_node) {
    int ret = 0;

    for_each_gpu(_node) {
        if (manual_pwm(_node) < 0) {
            errno_printf(1, "Error to set manual mode in pwm");
        } else
            ret = 1;
    }

    return ret;
}

int pwm_control(struct node_t *_node)
{
    int ret = 0;

    for_each_gpu(_node) {
        if (set_speed_matrix(_node) < 0) {
            errno_printf(1, "Error to set %s node", get_root(_node));
        } else
            ret = 1;
    }

    return ret;
}

