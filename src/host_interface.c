#include "host_interface.h"

#include <ctype.h>
#include <stdarg.h>

#include <stdlib.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/posix/unistd.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/version.h>

static host_interface_mode_t mode_s = HOST_INTERFACE_TEXT;

static host_text_cb text_cb_s = NULL;
static host_bin_cb bin_cb_s   = NULL;

static const struct device *const shell_uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
static const struct shell *shell_s;

static int cmd_app(const struct shell *sh, size_t argc, const char **argv)
{
    if (text_cb_s) {
        return text_cb_s(argc, argv);
    }
    return 0;
}

static int cmd_version(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "Zephyr version %s", KERNEL_VERSION_STRING);
    shell_print(sh, "Application version %s", "1.0");

    return 0;
}

static void bypass_cb(const struct shell *sh, uint8_t *data, size_t len)
{
    if (bin_cb_s) {
        bin_cb_s(data, len);
    }

    return;
}

SHELL_CMD_ARG_REGISTER(version, NULL, "Show app version", cmd_version, 1, 0);

SHELL_CMD_ARG_REGISTER(app, NULL, "App command", cmd_app, 1, 99);

void host_set_text_cb(host_text_cb cb)
{
    text_cb_s = cb;
}

void host_set_bin_cb(host_bin_cb cb)
{
    bin_cb_s = cb;
}

void host_send_text(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    shell_vfprintf(shell_s, SHELL_NORMAL, fmt, args);

    va_end(args);
}

void host_send_bin(const uint8_t *data, size_t len)
{
    for (int i = 0; i < len; i++) {
        uart_poll_out(shell_uart, data[i]);
    }
}

int host_set_mode(host_interface_mode_t mode)
{
    if (mode != mode_s) {
        mode_s = mode;
        if (mode == HOST_INTERFACE_TEXT) {
            shell_set_bypass(shell_s, NULL);
            return 0;
        }
        else if (mode == HOST_INTERFACE_BIN) {
            shell_set_bypass(shell_s, bypass_cb);
            return 0;
        }
    }
    return -1;
}

host_interface_mode_t host_get_mode()
{
    return mode_s;
}

int host_interface_init()
{
    shell_s = shell_backend_uart_get_ptr();
    return 0;
}
