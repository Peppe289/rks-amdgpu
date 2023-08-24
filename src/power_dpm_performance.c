#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "public.h"

/**
 * Referring to the linux kernel we can set various
 * parameters in the power_dpm_force_performance_level node.
 *
 * REF: https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/gpu/drm/amd/pm/amdgpu_pm.c#n164
 *
 * DOC: power_dpm_force_performance_level
 *
 * The amdgpu driver provides a sysfs API for adjusting certain power
 * related parameters.  The file power_dpm_force_performance_level is
 * used for this.  It accepts the following arguments:
 *
 * - auto
 *
 * - low
 *
 * - high
 *
 * - manual
 *
 * - profile_standard
 *
 * - profile_min_sclk
 *
 * - profile_min_mclk
 *
 * - profile_peak
 *
 * auto
 *
 * When auto is selected, the driver will attempt to dynamically select
 * the optimal power profile for current conditions in the driver.
 *
 * low
 *
 * When low is selected, the clocks are forced to the lowest power state.
 *
 * high
 *
 * When high is selected, the clocks are forced to the highest power state.
 *
 * manual
 *
 * When manual is selected, the user can manually adjust which power states
 * are enabled for each clock domain via the sysfs pp_dpm_mclk, pp_dpm_sclk,
 * and pp_dpm_pcie files and adjust the power state transition heuristics
 * via the pp_power_profile_mode sysfs file.
 *
 * profile_standard
 * profile_min_sclk
 * profile_min_mclk
 * profile_peak
 *
 * When the profiling modes are selected, clock and power gating are
 * disabled and the clocks are set for different profiling cases. This
 * mode is recommended for profiling specific work loads where you do
 * not want clock or power gating for clock fluctuation to interfere
 * with your results. profile_standard sets the clocks to a fixed clock
 * level which varies from asic to asic.  profile_min_sclk forces the sclk
 * to the lowest level.  profile_min_mclk forces the mclk to the lowest level.
 * profile_peak sets all clocks (mclk, sclk, pcie) to the highest levels.
 *
 */

enum power_dpm
{
    AUTO = 0,
    LOW,
    HIGH,
    MANUAL,
    PROFILE_STANDARD,
    PROFILE_MIN_SCLK,
    PROFILE_MIN_MCLK,
    PROFILE_PEAK,
    POWER_DPM_SIZE,
};

/**
 * List of accepted values ​​in the node
 * /sys/class/drm/card0/device/power_dpm_force_performance_level
 */
const char *power_dpm_value[POWER_DPM_SIZE] = {
    [AUTO] = "auto",
    [LOW] = "low",
    [HIGH] = "high",
    [MANUAL] = "manual",
    [PROFILE_STANDARD] = "profile_standard",
    [PROFILE_MIN_SCLK] = "profile_min_sclk",
    [PROFILE_MIN_MCLK] = "profile_min_mclk",
    [PROFILE_PEAK] = "profile_peak",
};

/**
 * Check if the input setting is valid (present in the accepted values).
 */
static int validate_input_power_dpm_performance(const char *setting) {
    int index;

    for (index = 0; index < POWER_DPM_SIZE; ++index) {
        if (strcmp(setting, power_dpm_value[index]) == 0) {
            return 0;
        }
    }

    return -1;
}

/**
 * Set the node setting.
 * For efficiency before overwriting check the validity and check if that setting
 * is already set.
 */
int set_power_dpm_performance(const char *setting)
{
    FILE *fp;
    char path[128];
    char buf[20];

    /**
     * To avoid errors, set the cell values ​​to 0.
     * The chances of error are nearly impossible, but better to be sure
     */
    memset(path, 0, sizeof(path));
    strcpy(path, "/sys/class/drm/card0/device/");
    strcat(path, configurable_data[POWER_DPM_FORCE_PERFORMANCE_LEVEL]);

    fp = fopen(path, "r+");
    if (fp == NULL)
        return -1;

    fscanf(fp, "%s", buf);
    if (strcmp(buf, setting) == 0)
    {
        fclose(fp);
        return 0;
    }

    if (validate_input_power_dpm_performance(setting) != 0) {
        print_err("Value %s not valid for %s\n", setting, path);
        fclose(fp);
        return -1;
    }

    if (fprintf(fp, "%s", setting) < 0) {
        print_err("Error to write %s in %s\n", setting, path);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}