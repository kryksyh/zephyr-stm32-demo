#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_SENSOR_COUNT 256

void sensor_init();
void sensor_poll();

float sensor_get_temp(uint32_t idx);
int32_t sensor_get_offset(uint32_t idx);
float sensor_get_coef(uint32_t idx);
uint32_t sensor_get_interval(uint32_t idx);
bool sensor_get_enabled(uint32_t idx);

int sensor_set_config(uint32_t idx, uint32_t interval, int32_t offset, float coef);

int sensor_set_offset(uint32_t idx, int32_t offset);
int sensor_set_coef(uint32_t idx, float coef);
int sensor_set_interval(uint32_t idx, uint32_t interval);
int sensor_set_enabled(uint32_t idx, bool enabled);
int sensor_refresh_data(uint32_t idx);
