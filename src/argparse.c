#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "data_struct.h"



static
enum argparse_option_type verify_option_arg(struct argparse_option *option, char arg[PATH_MAX], int *ret) {
    int i;
    size_t length;

    for (i = 0; option[i].type != ARG_PARSE_END; ++i) {
        length = strlen(option[i].long_name);
        if ((strncmp(option[i].long_name, arg, length) == 0) &&
            (strlen(arg) > (length + 1))) {
            memcpy(arg, &arg[length + 1], PATH_MAX);
            *ret = option[i].id;
            return option[i].type;
        }
    }

    return -1;
}

static int validate_input_fan1_speed(const char *arg) {
    int i, k, lenght;
    int flag = 0;
    // all valid value for input
    char valid[] = {'0', '1', '2', '3', '4', '5', '6',
                            '7', '8', '9', ':', '[', ']', ','};

    char *data = (char *)arg;
    data += strcspn(arg, "=") + 1;
    lenght = strlen(data);

    // If there are values ​​other than the stable ones you exit with an error
    for (i = 0; i != lenght; ++i) {
        flag = 0;
        for (k = 0; k != sizeof(valid); ++k) {
            if (valid[k] == data[i])
                flag = 1;
        }

        if (flag == 0)
            return -1;
    }

    // check the syntax
    for (flag = 0, i = 0; i != lenght; ++i) {
        if (data[i] == '[') {
            if (flag == 0) flag = 1;
            else return -1;
        } else if (data[i] == ']') {
            if (flag == 1) flag = 0;
            else return -1;
        }
    }

    return 0;
}

int init_fan1_speed(struct amdgpu_fan1 **fan_speed, void *data) {
    int ret;
    char *ptr = data;
    int i = 0;
    struct amdgpu_fan1 *self;

    ret = validate_input_fan1_speed(data);
    if (ret < 0) {
        err_printf("Illegal value\n");
        return ret;
    }

    self = malloc(sizeof(struct amdgpu_fan1));

    while (sscanf(ptr, "[%d:%d]", &(self[i].thermal), &(self[i].fan_speed)) == 2) {
        i++;
        ptr += strcspn(ptr, ",") + 1;
        self = realloc(self, (i + 1) * sizeof(struct amdgpu_fan1));
    }
    self[i].fan_speed = -1;
    self[i].thermal = -1;

#if 0
    int step;

    // find limit
    for (step = 0; (self[step].thermal != -1) &&
            (self[step].fan_speed != -1); step++);

    for (i = 0; i < step; i++)
    {
        printf("%d - %d\n\n", self[i].fan_speed, self[i].thermal);
    }
#endif

    *fan_speed = self;
    return ret;
}

int __int_args(struct argparse_option *option, struct node_t *amdgpu,
                struct amdgpu_fan1 **fan_speed, int argc, char *argv[])
{
    struct argparse self;
    char *arg;
    char calc_arg[PATH_MAX] = {0};
    int ret, exit_val;

    fptr function[] = {
        [OPT_SET_PWM_MODE] = &pwm_set,
        [OPT_SET_FAN] = NULL,
    };

    memset(&self, 0, sizeof(struct argparse));
    self.argc = argc - 1;
    self.argv = &argv[1];

    if (argc <= 1)
        return 0; // no parameters, go with the default settings

    for (; self.argc; self.argc--, self.argv++)
    {
        arg = self.argv[0];
        // verify is other than '--'
        if ((strlen(arg) < 2) || (arg[0] != '-') || (arg[1] != '-'))
        {
            err_print("Unknow command %s\n", arg);
            continue;
        }
        
        arg += 2; // skipp '--'
        memcpy(calc_arg, arg, strlen(arg) + 1);

        // find end return type of arg
        switch (verify_option_arg(option, calc_arg, &ret))
        {
            case ARG_PARSE_INT:
                exit_val = function[ret](amdgpu, calc_arg);
                if (exit_val < 0)
                    return exit_val;
                break;
            case ARG_PARSE_INT_ARRAY:
                if (ret == OPT_SET_FAN) {
                    exit_val = init_fan1_speed(fan_speed, calc_arg);
                    if (exit_val < 0) {
                        return exit_val;
                    } else {
                        return 0; // in this case, continue
                    }
                }
                break;
            default:
                err_print("Unknow command %s\n", arg);
                break;
        }
    }

    return 1;
}