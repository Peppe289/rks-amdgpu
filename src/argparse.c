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
            strcpy(arg, &arg[length + 1]);
            *ret = option[i].id;
            return option[i].type;
        }
    }

    return -1;
}

int __int_args(struct argparse_option *option,
                struct node_t *amdgpu, int argc, char *argv[])
{
    struct argparse self;
    char *arg;
    char calc_arg[PATH_MAX] = {0};
    int ret;

    fptr function[] = {
        [OPT_SET_PWM_MODE] = &pwm_set,
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
                function[ret](amdgpu, calc_arg);
                break;
            case ARG_PARSE_INT_ARRAY:

                break;
            default:
                err_print("Unknow command %s\n", arg);
                break;
        }
    }

    return 1;
}