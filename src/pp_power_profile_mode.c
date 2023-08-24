#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "public.h"

enum profile_mode {
    BOOTUP_DEFAULT = 0,
    POWER_3D_FULL_SCREEN,
    POWER_SAVING,
    VIDEO,
    VR,
    COMPUTE,
    CUSTOM,
    SIZE_POWER,
};

const char *power_profile_mode[SIZE_POWER] = {
    [BOOTUP_DEFAULT] = "BOOTUP_DEFAULT",
    [POWER_3D_FULL_SCREEN] = "3D_FULL_SCREEN",
    [POWER_SAVING] = "POWER_SAVING",
    [VIDEO] = "VIDEO",
    [VR] = "VR",
    [COMPUTE] = "COMPUTE",
    [CUSTOM] = "CUSTOM"
};

/**
 * Check if the input setting is valid (present in the accepted values).
 */
static int validate_input_power_profile_mode(const char *setting) {
    int index;

    for (index = 0; index < SIZE_POWER; ++index) {
        if (strcmp(setting, power_profile_mode[index]) == 0) {
            return index;
        }
    }

    return -1;
}

/**
 * With this sysfs:
 * Has as output the table of acceptable values ​​and possible configurations
 * (some problems with the integrated ones).
 * 
 * In input receives a number indicating the mode.
 */
int pp_power_profile_mode(const char *setting) {

    FILE *fp;
    char path[128], buf[20];
    int index;

    /**
     * To avoid errors, set the cell values ​​to 0.
     * The chances of error are nearly impossible, but better to be sure
     */
    memset(path, 0, sizeof(path));
    strcpy(path, "/sys/class/drm/card0/device/");
    strcat(path, configurable_data[PP_POWER_PROFILE_MODE]);

    fp = fopen(path, "r+");
    if (fp == NULL)
        return -1;
    
    index = validate_input_power_profile_mode(setting);
    if (index < 0) {
        print_err("Value %s not valid for %s\n", setting, path);
        fclose(fp);
        return -1;
    }

    if (fprintf(fp, "%d", index) < 0) {
        print_err("Error to write %d in %s\n", index, path);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}