#include <stdio.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "data_struct.h"
#include "utils.h"

/**
 * @def_gpu_path: is path for all GPU. Replace X with number of GPU.
 */
const char *def_gpu_path = "/sys/class/drm/";

/**
 * Check which node is present inside the hwmon path,
 * then read and make sure it is AMDGPU.
 *
 * Return 1 if all is ok, 0 if not.
 */
static int validate_amdgpu_path(const char *path)
{
    DIR *d_root;
    struct dirent *dir;
    char c_path[255] = {0};
    size_t length;
    FILE *fp;
    char s_data[32];

    // sscanf(path, "/sys/class/drm/%s/device/hwmon/", c_path);
    strcat(c_path, "/sys/class/drm/");
    strcat(c_path, path);
    strcat(c_path, "/device/hwmon/");
    length = strlen(c_path);

    /**
     * we are here:
     * /sys/class/drm/cardX/device/hwmon/
     *
     * Now open this and found "DIR/hwmonY".
     * In the calling function we have defined the X.
     * Now we need to find the right Y and copy in c_path.
     *
     * N.B. It seems it is possible to have only one hwmonY in this path.
     */
    d_root = opendir(c_path);
    validate_alloc(d_root);
    while ((dir = readdir(d_root)) != NULL)
    {
        if (strncmp(dir->d_name, "hwmon", strlen("hwmon")) == 0)
        {
            memcpy(&c_path[length], dir->d_name, strlen(dir->d_name) + 1);
            break;
        }
    }

    closedir(d_root);
    strcat(c_path, "/name"); // end create path

    fp = fopen(c_path, "r");
    validate_alloc(fp);

    fscanf(fp, "%s", s_data);
    fclose(fp);

    if (strcmp(s_data, "amdgpu") != 0)
        return 0;

    return 1;
}

/**
 * @ret: return list of GPU.
 * @ppath: Possible GPU node.
 *
 * If there are multiple GPUs then the @gpu_path is NULL
 * and returns an list with the IDs. If not, return NULL.
 */
static struct node_t *search_for_gpu(void)
{
    struct node_t *amdgpu = NULL;
    const char *ppath = "cardX";
    size_t path_len = strlen(def_gpu_path);
    // for search dir
    DIR *d_root;
    struct dirent *dir;
    char *t_data;

    d_root = opendir(def_gpu_path);
    validate_alloc(d_root);

    while ((dir = readdir(d_root)) != NULL)
    {
        if (amdgpu == NULL)
            amdgpu = new_node();

        /**
         * In this if: remove X of ppath and compare.
         * Also, compare if is same length,
         * for example: cardX have same length of card3
         */
        if ((strncmp(dir->d_name, ppath, strlen(ppath) - 1) == 0) &&
            (strlen(dir->d_name) == strlen(ppath)))
        {
            if (!validate_amdgpu_path(dir->d_name))
            {
                print_info_func("Found non AMDGPU at %s. Skipp\n", dir->d_name);
                continue;
            }

            t_data = malloc((path_len + strlen(dir->d_name) + 1) * sizeof(char));
            memcpy(t_data, def_gpu_path, strlen(def_gpu_path) + 1);
            strcat(t_data, dir->d_name);
            push(&amdgpu, t_data);
        }
    }

    closedir(d_root);
    return amdgpu;
}

static void show_amdgpu_list(struct node_t *_node)
{
    while (_node != NULL)
    {
        if (_node->data != NULL)
            print_info_func("Save in list %s node\n", _node->data);

        _node = _node->next;
    }
}

int main()
{
    struct node_t *amdgpu;
    amdgpu = search_for_gpu();
    show_amdgpu_list(amdgpu);
    destroy_node(&amdgpu);

    return EXIT_SUCCESS;
}