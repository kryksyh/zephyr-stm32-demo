#include "conf.h"

// zephyr inc, feel free to re-implement any of the macros used
#include <zephyr/sys/util.h>

static sensor_conf_t sensor_conf_s[] = { {
    .interval   = 1000,
    .cal_offset = 0,
    .cal_coef   = 1,
} };

bool conf_init()
{
    // Initialize hardware / interface / whatever
    return true;
}

bool conf_get_sensor(size_t idx, sensor_conf_t *sensor_conf)
{
    if (idx >= ARRAY_SIZE(sensor_conf_s)) {
        return false;
    }
    *sensor_conf = sensor_conf_s[idx];
    return true;
}

bool conf_set_sensor(size_t idx, const sensor_conf_t *sensor_conf)
{
    if (idx >= ARRAY_SIZE(sensor_conf_s)) {
        return false;
    }
    // save the data
    return true;
}
