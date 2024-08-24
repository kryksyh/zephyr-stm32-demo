/*
 * temp_reader
 *
 * Copyright (c) [2024] [DM]
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <stdbool.h>
#include "app.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

int main(void)
{
    LOG_INF("Starting...");
    app_init();
    app_run();
    return 0;
}
