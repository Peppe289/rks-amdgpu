/**
 * This module takes care of reading the config
 * file and sending the correct settings to the other modules.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

static void collect_config_settings(char *buf, char *config, char *name) {
    int size;
    int i;
    int k;

    /**
     * TODO: replace this system whit sscanf(buf, "%s", input_str)
     */
    for (i = 0, k = 0; i < sizeof(buf); i++) {
        /**
         * In case the stupid user puts the string
         * after a some number of spaces.
         * 
         * or break at first space after word. 
         */
        if (buf[i] == ' ' && k != 0)
            break;
        else if (buf[i] == ' ')
            continue;

        name[k] = buf[i];
        k++;
    }

    // add the end of the string for correct reading
    name[k] = '\0';

    k = 0;
    // Do not initialize the index (i). The reading of the previous stream continues.
    for (; i != sizeof(buf); ++i) {
        if (buf[i] == ' ' || buf[i] == '=') continue;

        config[k] = buf[i];
    }

    config[k] = '\0';
}

static int set_config_data(int index, char *config) {

    int ret;

    switch (index)
    {
    case PP_OD_CLK_VOLTAGE:
        
        break;
    case POWER_DPM_FORCE_PERFORMANCE_LEVEL:

        break;
    
    case PP_DPM_SCLK:

        break;

    case PP_DPM_MCLK:

        break;

    case PP_POWER_PROFILE_MODE:

        break;
    default:
        ret = -1;
        fprintf(stderr, "No valid data\n");
        break;
    }

    return ret;
}

static int get_index_config_data(FILE *fp, char *buf, char *config)
{
    int index;
    char *tmp[128];
    
    collect_config_settings(buf, config, tmp);

    for (index = 0; index < SIZE_CONFIG_DATA; ++index)
    {
        if (strcmp(tmp, configurable_data[index]))
        {
            return index;
        }
    }

    return -1;
}

/**
 * return value is for error.
 */
static int load_configuration_file(char *path)
{

    FILE *fp;
    char buf[255];
    int index;
    char config[128] = {0};
    int line;
    int result;

    fp = fopen(path, "r");
    if (fp == NULL)
        return -1;

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        line++;
        if (buf[0] == '#') continue;

        index = get_index_config_data(fp, buf, config);
        if (index == -1) continue;

        result = set_config_data(index, config);
        if (result == -1)
            fprintf(stderr, "Error to set line %d\n", line);
    }

    return 0;
}