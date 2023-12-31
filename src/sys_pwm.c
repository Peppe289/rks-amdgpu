/**
 * Via sysctl manage the fan pwm.
 * This module allows you to fully manage the fan of your AMD GPU.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "data_struct.h"
#include "utils.h"

const int speed_default[][6] = {
    {0, 30, 50, 60, 70, 74}, // temp
    {25, 25, 30, 40, 70, 100}, // speed
};

#define FAN_STEPS   6

void free_pgpu(struct p_gpu *data)
{

    if (data->hwmon != NULL)
        free(data->hwmon);

    if (data->root != NULL)
        free(data->root);

    free(data);
}

const char *get_root(struct node_t *_node)
{
    struct p_gpu *data = _node->data;

    if (data != NULL)
        return data->root;

    return NULL;
}

const char *get_hwmon(struct node_t *_node)
{
    struct p_gpu *data = _node->data;

    if (data != NULL)
        return data->hwmon;

    return NULL;
}

static int manual_pwm(struct node_t *_node, int mode)
{
    int fd, ret;
    char path[PATH_MAX], buffer[10] = {0};
    const char *hwmon = get_hwmon(_node);

    if (hwmon == NULL)
        return -1;

    memcpy(path, hwmon, strlen(hwmon) + 1);
    strcat(path, "pwm1_enable"); // for now controll only first node

    if ((fd = open(path, O_RDWR)) < 0)
    {
        errno_printf(1, "Error to open %s", path);
        return -1;
    }

    if ((ret = read(fd, buffer, sizeof(buffer))) < 0)
    {
        errno_printf(1, "Error to read %s", path);
        return -1;
    }

    buffer[ret] = '\0';
    sscanf(buffer, "%d", &ret);

    if (ret == mode)
    {
        close(fd);
        return 0; // already set to @mode
    }

    memset(buffer, 0, sizeof(buffer));   // clear buffer
    sprintf(buffer, "%d", mode); // set to @mode

    if (write(fd, buffer, sizeof(buffer)) < 0)
    {
        errno_printf(1, "Error to set %s", path);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int pwm_set(struct node_t *_node, void *mode) {
    int set;
    const char *ptr = mode;
    sscanf(ptr, "%d", &set);

    if (!(set <= AMD_FAN_CTRL_AUTO)) {
        err_printf("Error to set pwm_mode: illegal value\n");
        return -1;
    }

    return manual_pwm(_node, set);
}

static int get_thermal(struct node_t *_node)
{
    int fd, ret;
    char path[PATH_MAX], buffer[10];

    memcpy(path, get_hwmon(_node), strlen(get_hwmon(_node)) + 1);
    strcat(path, "temp1_input"); // for now controll only first node

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        errno_printf(1, "Error to open %s", path);
        return -1;
    }

    if ((ret = read(fd, buffer, sizeof(buffer))) < 0)
    {
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
static int getIndex_Therm(int therm, struct amdgpu_fan1 *fan_speed)
{
    int step, index;

    // find limit
    for (step = 0; (fan_speed[step].thermal != -1) &&
            (fan_speed[step].fan_speed != -1); step++);

    for (index = 0; index < step - 1; index++)
    {
        if (therm > fan_speed[index].thermal && therm <= fan_speed[index + 1].thermal)
            return index;
    }

    return step - 1;
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
static int u_fanspeed(int therm, int index, struct amdgpu_fan1 *fan_speed)
{
    int x1 = fan_speed[index].thermal;
    int x2 = fan_speed[index + 1].thermal;
    int y1 = fan_speed[index].fan_speed;
    int y2 = fan_speed[index + 1].fan_speed;
    int x = therm;
    int m = (y2 - y1) / (x2 - x1);
    int q = y1 - m * x1;

    return m * x + q;
}

static int set_speed_matrix(struct node_t *_node, struct amdgpu_fan1 *fan_speed)
{
    int fd, data, therm, index;
    char path[PATH_MAX], buff[10] = {0};

    memcpy(path, get_hwmon(_node), strlen(get_hwmon(_node)) + 1);
    strcat(path, "pwm1"); // for now controll only first node

    if ((therm = get_thermal(_node)) < 0)
    {
        errno_printf(1, "Error to get thermal info"); // collect temp information
        return -1;
    }

    therm = therm / 1000;
    index = getIndex_Therm(therm, fan_speed); // get index of matrix for set fan

    data = u_fanspeed(therm, index, fan_speed);
    info_printf("Set %d percentage with %d temp\n", data, therm);
    data = data * 2.55;

    if ((fd = open(path, O_WRONLY)) < 0)
    {
        return -1;
    }

    sprintf(buff, "%d", data);

    if (write(fd, buff, strlen(buff) + 1) < 0)
    {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int pwm_init(struct node_t *_node)
{
    int ret = 0;

    for_each_gpu(_node)
    {
        if (manual_pwm(_node, AMD_FAN_CTRL_MANUAL) < 0)
        {
            errno_printf(1, "Error to set manual mode in pwm");
        }
        else
            ret = 1;
    }

    return ret;
}

static struct amdgpu_fan1 *init_amdgpu_fan1_speed(struct amdgpu_fan1 *fan_speed) {
    int i;

    if (fan_speed != NULL)
        return fan_speed;

    fan_speed = malloc(sizeof(struct amdgpu_fan1) * (FAN_STEPS + 1));

    for (i = 0; i != FAN_STEPS; ++i) {
        fan_speed[i].thermal = speed_default[0][i];
        fan_speed[i].fan_speed = speed_default[1][i];
    }

    /**
     * Values ​​agreeing with 1 declare that we are at the
     * end of the array. it was the most generic way
     * to declare termination.
     */    
    fan_speed[i].thermal = -1;
    fan_speed[i].fan_speed = -1;

    return fan_speed;
}

int pwm_control(struct node_t *_node, struct amdgpu_fan1 **fan_speed)
{
    int ret = 0;

    if (*fan_speed == NULL)
        *fan_speed = init_amdgpu_fan1_speed(*fan_speed);

    for_each_gpu(_node)
    {
        if (set_speed_matrix(_node, *fan_speed) < 0)
        {
            errno_printf(1, "Error to set %s node", get_root(_node));
        }
        else
            ret = 1;
    }

    signal(SIGINT, interruptHandler);

    return ret & running;
}
