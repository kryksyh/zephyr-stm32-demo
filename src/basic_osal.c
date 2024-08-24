/**
 * temp_reader
 *
 * Copyright (c) [2024] [DM]
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include "basic_osal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// Zephyr
#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(stack0, STACK_SIZE);

int low_level_init()
{
    if (usb_enable(NULL)) {
        return -1;
    }
    return 0;
}

int osal_setup_timer(uint32_t time_ms, timer_cb cb)
{
    // TODO
    return 0;
}

int osal_sleep_ms(uint32_t time_ms)
{
    k_sleep(K_MSEC(time_ms));
    return 0;
}

int osal_sleep_ms_until(uint32_t time_ms, void *sem)
{
    struct k_sem *semaphore = sem;
    int ret                 = k_sem_take(semaphore, K_MSEC(time_ms));
    return ret;
}

// Thats correct, we can only create a single semaphore
// no time
static struct k_sem _sem;
void *osal_sem_create()
{
    struct k_sem *sem = &_sem;
    if (sem == NULL) {
        return NULL;
    }

    k_sem_init(sem, 0, 1);
    return sem;
}

int osal_sem_give(void *sem)
{
    if (sem == NULL) {
        return -EINVAL;
    }

    k_sem_give((struct k_sem *) sem);
    return 0;
}

int osal_sem_take(void *sem, uint32_t time_ms)
{
    if (sem == NULL) {
        return -EINVAL;
    }

    return k_sem_take((struct k_sem *) sem, K_MSEC(time_ms));
}

uint32_t osal_get_system_time_ms(void)
{
    return k_uptime_get_32();
}

void *osal_start_thread(
    void (*entry_func)(void *), void *param, void *stack, size_t stack_size, int priority)
{
    static struct k_thread thread_data;

    if (stack_size < STACK_SIZE) {
        printk("Stack size is too small.\n");
        return NULL;
    }

    k_thread_create(&thread_data,
                    stack0,
                    sizeof stack0,
                    (k_thread_entry_t) entry_func,
                    param,
                    NULL,
                    NULL,
                    priority,
                    0,
                    K_NO_WAIT);

    return &thread_data;
}