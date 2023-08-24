/**
 * This module takes care of reading the config
 * file and sending the correct settings to the other modules.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "public.h"

char *configurable_data[SIZE_CONFIG_DATA] = {
    [PP_OD_CLK_VOLTAGE] = "pp_od_clk_voltage",
    [POWER_DPM_FORCE_PERFORMANCE_LEVEL] = "power_dpm_force_performance_level",
    [PP_DPM_SCLK] = "pp_dpm_sclk",
    [PP_DPM_MCLK] = "pp_dpm_mclk",
    [PP_POWER_PROFILE_MODE] = "pp_power_profile_mode",
};

static void collect_config_settings(char *buf, char *config, char *name) {
    int size;
    int i;
    int k;

    /**
     * TODO: replace this system whit sscanf(buf, "%s", input_str)
     */
    for (i = 0, k = 0; i < strlen(buf); i++) {
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
    for (; i != strlen(buf); ++i) {
        if (buf[i] == ' ' || buf[i] == '=' || buf[i] == '\n') continue;

        config[k] = buf[i];
        k++;
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
        ret = set_power_dpm_performance(config);
        break;
    
    case PP_DPM_SCLK:

        break;

    case PP_DPM_MCLK:

        break;

    case PP_POWER_PROFILE_MODE:
        ret = pp_power_profile_mode(config);
        break;
    default:
        ret = -1;
        print_err("No valid data\n");
        break;
    }

    return ret;
}

static int get_index_config_data(FILE *fp, char *buf, char *config)
{
    int index;
    char tmp[128];
    
    collect_config_settings(buf, config, tmp);

    for (index = 0; index < SIZE_CONFIG_DATA; ++index)
    {
        if (strcmp(tmp, configurable_data[index]) == 0)
        {
            return index;
        }
    }

    return -1;
}

/**
 * return value is for error.
 */
int load_configuration_file(char *path)
{

    FILE *fp;
    char buf[255];
    int index;
    char config[128] = {0};
    int line = 0;
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
            print_err("Error to set line %d\n", line);
    }

    fclose(fp);
    return 0;
}