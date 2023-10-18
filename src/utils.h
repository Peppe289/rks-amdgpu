#ifndef __RKS_UTILS_H__
#define __RKS_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef PATH_MAX
#undef PATH_MAX
#endif
#define PATH_MAX    (100)

#define err_print(...) fprintf(stderr, __VA_ARGS__);
#define info_print(...) fprintf(stdout, __VA_ARGS__);

#define errno_printf(x, ...)                            \
    do                                                  \
    {                                                   \
        fprintf(stderr, "%s: ", __func__);              \
        err_print(__VA_ARGS__);                         \
        if (x)                                          \
        {                                               \
            fprintf(stderr, ": %s\n", strerror(errno)); \
        }                                               \
    } while (0);

#define info_printf(...)                   \
    do                                     \
    {                                      \
        fprintf(stdout, "%s: ", __func__); \
        info_print(__VA_ARGS__)            \
    } while (0);

#define err_printf(...) errno_printf(0, __VA_ARGS__)

#define for_each_gpu(x) for (; x != NULL; x = x->next)

#endif