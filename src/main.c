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
