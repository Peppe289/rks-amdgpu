#ifndef _AMD_UTILS_H_
#define _AMD_UTILS_H_

#include <stdio.h>
#include <stdlib.h>

enum config_data
{
    PP_OD_CLK_VOLTAGE = 0,
    POWER_DPM_FORCE_PERFORMANCE_LEVEL,
    PP_DPM_SCLK,
    PP_DPM_MCLK,
    PP_POWER_PROFILE_MODE,
    SIZE_CONFIG_DATA,
};

const char *configurable_data[] = {
    [PP_OD_CLK_VOLTAGE] = "pp_od_clk_voltage",
    [POWER_DPM_FORCE_PERFORMANCE_LEVEL] = "power_dpm_force_performance_level",
    [PP_DPM_SCLK] = "pp_dpm_sclk",
    [PP_DPM_MCLK] = "pp_dpm_mclk",
    [PP_POWER_PROFILE_MODE] = "pp_power_profile_mode",
};

#endif // _AMD_UTILS_H_
