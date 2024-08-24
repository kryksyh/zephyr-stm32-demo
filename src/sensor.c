/**
 * temp_reader
 *
 * Copyright (c) [2024] [DM]
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "sensor.h"

#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <zephyr/logging/log.h>
#include "basic_osal.h"
#include "conf.h"

LOG_MODULE_REGISTER(sensor);

static void sensor_read(size_t idx);

// Here I chose to not use any heavy locking such as mutexes or condvars
// all the data that could be accessed by several threads is stored in atomics
//
// deliberatly kept as standalone arrays instead of array of a struct
static atomic_int sensor_reading[MAX_SENSOR_COUNT];
static atomic_int sensor_timeout[MAX_SENSOR_COUNT];
static atomic_uint sensor_read_freq[MAX_SENSOR_COUNT];
static atomic_bool sensor_enabled[MAX_SENSOR_COUNT];

// these two are only accesed from a single thread
// no need for extra protection.
// If the sensor data will be read from any thread other than
// host interface, you would need to provide locking
static float sensor_coef[MAX_SENSOR_COUNT];
static int32_t sensor_offset[MAX_SENSOR_COUNT];
static void *sem_s;

void sensor_init()
{
    sem_s = osal_sem_create();
    for (size_t i = 0; i < MAX_SENSOR_COUNT; i++) {
        sensor_conf_t conf;
        bool res = conf_get_sensor(i, &conf);
        if (res) {
            atomic_init(&sensor_read_freq[i], conf.interval);
            atomic_init(&sensor_enabled[i], true);
            sensor_coef[i]   = conf.cal_coef;
            sensor_offset[i] = conf.cal_offset;
        }
        else {
            atomic_init(&sensor_read_freq[i], UINT32_MAX);
            atomic_init(&sensor_enabled[i], false);
            sensor_coef[i]   = 1;
            sensor_offset[i] = 0;
        }
    }
}

void sensor_poll()
{
    static uint32_t sleep_time = 0;
    while (true) {
        int32_t next_reading_delay = INT32_MAX;
        uint32_t cycle_begin       = osal_get_system_time_ms();
        for (size_t i = 0; i < MAX_SENSOR_COUNT; i++) {
            if (!atomic_load(&sensor_enabled[i])) {
                continue;
            }

            atomic_fetch_sub(&sensor_timeout[i], sleep_time);

            if (atomic_load(&sensor_timeout[i]) <= 0) {
                sensor_read(i);
                atomic_fetch_add(&sensor_timeout[i], atomic_load(&sensor_read_freq[i]));
                // if timeout is still negative, that probably means
                // that this sensor was just enabled, it is okay to just reset it
                if (atomic_load(&sensor_timeout[i]) <= 0) {
                    atomic_store(&sensor_timeout[i], atomic_load(&sensor_read_freq[i]));
                }
            }

            // calculate time for the next wake up
            int32_t time_left = atomic_load(&sensor_timeout[i]);
            if (time_left < next_reading_delay) {
                next_reading_delay = time_left;
            }
        }

        osal_sleep_ms_until(next_reading_delay, sem_s);
        sleep_time = osal_get_system_time_ms() - cycle_begin;
    }
}

float sensor_get_temp(uint32_t idx)
{
    if (atomic_load(&sensor_enabled[idx])) {
        int32_t temp = atomic_load(&sensor_reading[idx]);
        return (temp * sensor_coef[idx]) + sensor_offset[idx];
    }
    else {
        return NAN;
    }
}

int sensor_set_interval(uint32_t idx, uint32_t interval_ms)
{
    if (idx < 0 || idx >= MAX_SENSOR_COUNT) {
        return -1;
    }
    // If interval is longer then a day, it is probably an error
    if (interval_ms < 100 || interval_ms > 1 * 24 * 60 * 60 * 1000) {
        return -1;
    }
    // first update frequency
    atomic_store(&sensor_read_freq[idx], interval_ms);
    // then schedule for re-reading out of order
    atomic_store(&sensor_timeout[idx], 0);
    osal_sem_give(sem_s);
    return 0;
}

int sensor_set_config(uint32_t idx, uint32_t interval, int32_t offset, float coef)
{
    if (idx < 0 || idx >= MAX_SENSOR_COUNT) {
        return -1;
    }
    atomic_store(&sensor_read_freq[idx], interval);
    sensor_offset[idx] = offset;
    sensor_coef[idx]   = coef;

    return 0;
}

int32_t sensor_get_offset(uint32_t idx)
{
    return sensor_offset[idx];
}

float sensor_get_coef(uint32_t idx)
{
    return sensor_coef[idx];
}

uint32_t sensor_get_interval(uint32_t idx)
{
    return atomic_load(&sensor_read_freq[idx]);
}

int sensor_set_offset(uint32_t idx, int32_t offset)
{
    if (idx < 0 || idx >= MAX_SENSOR_COUNT) {
        return -1;
    }
    sensor_offset[idx] = offset;

    return 0;
}

int sensor_set_coef(uint32_t idx, float coef)
{
    if (idx < 0 || idx >= MAX_SENSOR_COUNT) {
        return -1;
    }
    if (isnan(coef) || coef == 0) {
        return -1;
    }
    sensor_coef[idx] = coef;

    return 0;
}

int sensor_set_enabled(uint32_t idx, bool enabled)
{
    if (idx < 0 || idx >= MAX_SENSOR_COUNT) {
        return -1;
    }

    if (sensor_coef[idx] == 0 || isnan(sensor_coef[idx]) ||
        atomic_load(&sensor_read_freq[idx]) < 100) {
        return -1;
    }

    atomic_store(&sensor_enabled[idx], enabled);
    return 0;
}

bool sensor_get_enabled(uint32_t idx)
{
    if (idx < 0 || idx >= MAX_SENSOR_COUNT) {
        return false;
    }

    return atomic_load(&sensor_enabled[idx]);
}

int sensor_refresh_data(uint32_t idx)
{
    if (idx < 0 || idx >= MAX_SENSOR_COUNT) {
        return -1;
    }
    osal_sem_give(sem_s);
    return 0;
}

// Here you may access hardware, trigger adc, send i2c command, pend an event
static void sensor_read(size_t idx)
{
    // random data
    atomic_store(&sensor_reading[idx], rand() % 10 + 20);
    // incrementing counter useful to verify timings
    // atomic_fetch_add(&sensor_reading[idx], 1);
}
