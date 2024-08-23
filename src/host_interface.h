/**
 *
 */

#pragma once

#include "proto/message.pb.h"

typedef enum
{
    HOST_INTERFACE_TEXT,
    HOST_INTERFACE_BIN,
} host_interface_mode_t;

typedef int (*host_text_cb)(int argc, const char **argv);
typedef int (*host_bin_cb)(const uint8_t *data, size_t len);

int host_interface_init();

int host_set_mode(host_interface_mode_t mode);
host_interface_mode_t host_get_mode();

void host_set_text_cb(host_text_cb cb);
void host_set_bin_cb(host_bin_cb cb);

void host_send_text(const char *fmt, ...);
void host_send_bin(const uint8_t *data, size_t len);
