/**
 * Configuration provider
 *
 * Depending on the requirements of the project could interface
 * onboard flash/eeprom, request the data from host or anything else
 *
 * here we just fallback to static default config
 *
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t interval;
    int32_t cal_offset;
    float cal_coef;
} sensor_conf_t;

bool conf_init();
bool conf_get_sensor(size_t idx, sensor_conf_t *sensor_conf);
bool conf_set_sensor(size_t idx, const sensor_conf_t *sensor_conf);
