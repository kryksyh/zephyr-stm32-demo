/**
 * temp_reader
 *
 * Copyright (c) [2024] [DM]
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

/*
 * Configuration provider
 *
 * Depending on the requirements of the project could interface
 * onboard flash/eeprom, request the data from the host or anything else
 *
 * for now we just fallback to the static default config
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
    uint32_t _padding;
} sensor_conf_t;

bool conf_init();
bool conf_get_sensor(size_t idx, sensor_conf_t *sensor_conf);
bool conf_set_sensor(size_t idx, const sensor_conf_t *sensor_conf);
