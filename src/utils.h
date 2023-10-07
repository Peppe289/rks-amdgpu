#ifndef __RKS_UTILS_H__
#define __RKS_UTILS_H__

#include <stdio.h>
#include <stdlib.h>

#define print_err_func(...)                \
    do                                     \
    {                                      \
        fprintf(stderr, "%s: ", __func__); \
        fprintf(stderr, __VA_ARGS__);      \
    } while (0);

#define print_info_func(...)               \
    do                                     \
    {                                      \
        fprintf(stdout, "%s: ", __func__); \
        fprintf(stdout, __VA_ARGS__);      \
    } while (0);

#define print_err(...) fprintf(stderr, __VA_ARGS__);
#define print_info(...) fprintf(stdout, __VA_ARGS__);

#define validate_alloc(x)                             \
    do                                                 \
    {                                                  \
        if (x == NULL)                                 \
        {                                              \
            print_info_func("Error to alloc %s\n", #x); \
            exit(EXIT_FAILURE);                        \
        }                                              \
    } while (0);

#endif