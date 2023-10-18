#include <stdio.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

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
static char *validate_amdgpu_path(const char *path)
{
    DIR *d_root;
    struct dirent *dir;
    char c_path[PATH_MAX] = {0};
    size_t length;
    FILE *fp;
    char s_data[32];
    char *hwmon;

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
    if ((d_root = opendir(c_path)) == NULL) {
        errno_printf(1, "Error to open %s", c_path);
        return NULL;
    }

    while ((dir = readdir(d_root)) != NULL)
    {
        if (strncmp(dir->d_name, "hwmon", strlen("hwmon")) == 0)
        {
            memcpy(&c_path[length], dir->d_name, strlen(dir->d_name) + 1);
            break;
        }
    }

    closedir(d_root);
    strcat(c_path, "/");
    hwmon = malloc((strlen(c_path) + 1) * sizeof(char));
    strcpy(hwmon, c_path); // copy path of hwmon for return
    strcat(c_path, "name"); // end create path

    if ((fp = fopen(c_path, "r")) == NULL) {
        free(hwmon);
        return NULL;
    }

    fscanf(fp, "%s", s_data);
    fclose(fp);

    if (strcmp(s_data, "amdgpu") == 0)
        return hwmon;

    free(hwmon);
    return NULL;
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
    struct p_gpu *t_data;
    char *hwmon;

    if ((d_root = opendir(def_gpu_path)) == NULL)
        return NULL;

    while ((dir = readdir(d_root)) != NULL)
    {
        /**
         * In this if: remove X of ppath and compare.
         * Also, compare if is same length,
         * for example: cardX have same length of card3
         */
        if ((strncmp(dir->d_name, ppath, strlen(ppath) - 1) == 0) &&
            (strlen(dir->d_name) == strlen(ppath)))
        {
            if ((hwmon = validate_amdgpu_path(dir->d_name)) == NULL)
            {
                info_printf("Found non AMDGPU at %s. Skipp\n", dir->d_name);
                continue;
            }

            t_data = malloc(sizeof(struct p_gpu));
            t_data->root = malloc((path_len + strlen(dir->d_name) + 1) * sizeof(char));
            memcpy(t_data->root, def_gpu_path, strlen(def_gpu_path) + 1);
            strcat(t_data->root, dir->d_name);
            // hwmon already in heap
            t_data->hwmon = hwmon;

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
        if (_node->data != NULL) {
            info_printf("Save in list root %s node\n", get_root(_node));
            info_printf("Save in list hwmon %s node\n", get_hwmon(_node));
        }

        _node = _node->next;
    }
}

int main()
{
    struct node_t *amdgpu;
    if ((amdgpu = search_for_gpu()) == NULL) {
        err_printf("Error to init data\n");
        destroy_node(&amdgpu);
        exit(-1);
    }
    show_amdgpu_list(amdgpu);
    info_print("start with pid: %d\n", getpid());
    while (pwm_control(amdgpu)) {
        sleep(2);
    }
    destroy_node(&amdgpu);

    return EXIT_SUCCESS;
}