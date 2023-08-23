#ifndef _AMD_UTILS_H_
#define _AMD_UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#define print_err(...)                     \
    do                                     \
    {                                      \
        fprintf(stderr, "%s: ", __func__); \
        fprintf(stderr, __VA_ARGS__);      \
    } while (0);

#define print_info(...)                    \
    do                                     \
    {                                      \
        fprintf(stdout, "%s: ", __func__); \
        fprintf(stdout, __VA_ARGS__);      \
    } while (0);

enum config_data
{
    PP_OD_CLK_VOLTAGE = 0,
    POWER_DPM_FORCE_PERFORMANCE_LEVEL,
    PP_DPM_SCLK,
    PP_DPM_MCLK,
    PP_POWER_PROFILE_MODE,
    SIZE_CONFIG_DATA,
};

extern char *configurable_data[SIZE_CONFIG_DATA];

#endif // _AMD_UTILS_H_
