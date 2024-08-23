/**
 * This is a naive an barely usable OSAL wrapper for Zephyr.
 * Logging subsystem was left out intentionally
 *
 * To port the app to another OS, you will need to reimplement functions here to map
 * them to os primitives of your choice
 */
#pragma once

#include <ctype.h>
#include <stdint.h>

typedef void (*timer_cb)(void);

int low_level_init();

int osal_setup_timer(uint32_t time_ms, timer_cb cb);
int osal_sleep_ms(uint32_t time_ms);
int osal_sleep_ms_until(uint32_t time_ms, void *sem);
void *osal_sem_create();
int osal_sem_give(void *sem);
int osal_sem_take(void *sem, uint32_t time_ms);
uint32_t osal_get_system_time_ms();
void *osal_start_thread(
    void (*entry_func)(void *), void *param, void *stack, size_t stack_size, int priority);
