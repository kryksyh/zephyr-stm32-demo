#include "app.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>

#include "basic_osal.h"
#include "conf.h"
#include "host_interface.h"
#include "sensor.h"

#include "pb_decode.h"
#include "pb_encode.h"

LOG_MODULE_REGISTER(app);

#define THREAD_STACK_SIZE 1024

static uint8_t sensor_thread_stack[THREAD_STACK_SIZE];

static int read_sensor_text(int argc, const char **argv);
static int enable_sensor_text(int argc, const char **argv);
static int disable_sensor_text(int argc, const char **argv);
static int config_sensor_text(int argc, const char **argv);
static int toggle_interface_mode_text(int argc, const char **argv);

static int read_sensor_bin(const Request *request);
static int config_sensor_bin(const Request *request);
static int toggle_interface_mode_bin(const Request *request);

typedef int (*text_command_handler_t)(int argc, const char **argv);
typedef int (*bin_command_handler_t)(const Request *request);

typedef struct {
    char cmd[16];
    char desc[44];
    text_command_handler_t handler;
} text_command_t;

static text_command_t text_commands[] = {
    { "read", "Read sensor data. Ex: app read 1 2 3", read_sensor_text },
    { "enable", "Enable sensors. Ex: app enable 1 2 3", enable_sensor_text },
    { "disable", "Disable sensors. Ex: app disable 1 2 3", disable_sensor_text },
    { "config", "Configure sensor", config_sensor_text },
    { "toggle", "Switch interface to binary mode", toggle_interface_mode_text },
};

typedef struct {
    RequestType type;
    bin_command_handler_t handler;
} bin_command_t;

static bin_command_t bin_commands[] = {
    { RequestType_Toggle, toggle_interface_mode_bin },
    { RequestType_ReadData, read_sensor_bin },
    { RequestType_ConfigSensor, config_sensor_bin },
};

int process_host_text(int argc, const char **argv)
{
    int res = -1;
    if (argc < 2) {
        host_send_text("Please specify application command\n");
        for (int i = 0; i < sizeof text_commands / sizeof text_commands[0]; i++) {
            host_send_text("  * %-8s\t%s\n", text_commands[i].cmd, text_commands[i].desc);
        }
        return res;
    }
    bool handler_found = false;

    for (int i = 0; i < sizeof text_commands / sizeof text_commands[0]; i++) {
        if (strcmp(argv[1], text_commands[i].cmd) == 0) {
            res           = text_commands[i].handler(argc - 2, &argv[2]);
            handler_found = true;
            break;
        }
    }
    if (!handler_found) {
        host_send_text("Unknown command: %s\n", argv[1]);
    }

    return res;
}

int process_host_bin(const uint8_t *data, size_t len)
{
    Request request      = Request_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(data, len);

    int res     = -1;
    bool status = pb_decode(&istream, Request_fields, &request);

    if (!status) {
        host_send_text("Failed to decode request\n");
        return 1;
    }

    bool handler_found = false;

    for (int i = 0; i < sizeof bin_commands / sizeof bin_commands[0]; i++) {
        if (bin_commands[i].type == request.type) {
            res           = bin_commands[i].handler(&request);
            handler_found = true;
            break;
        }
    }
    if (!handler_found) {
        host_send_text("Unknown command: %d\n", request.type);
    }
    return res;
}

void app_init()
{
    low_level_init();

    host_interface_init();

    conf_init();

    sensor_init();

    osal_start_thread(sensor_poll, NULL, sensor_thread_stack, sizeof sensor_thread_stack, 5);

    // setup host interface
    host_set_text_cb(process_host_text);
    host_set_bin_cb(process_host_bin);
}

void app_run()
{
    while (true) {
        osal_sleep_ms(1000);
    }
}

static int read_sensor_text(int argc, const char **argv)
{
    host_send_text("Read command received\n");
    if (argc <= 0) {
        host_send_text("Please specify one or more sensor ids (space separated)\n");
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        uint32_t sensor_id = atoi(argv[i]);
        if (sensor_id < 0 || sensor_id >= MAX_SENSOR_COUNT) {
            continue;
        }
        // to skip floating formatting, lets do it manually
        float temp = sensor_get_temp(sensor_id);
        if (isnan(temp)) {
            host_send_text("\tSensor[%zd] is offline\n", sensor_id);
            continue;
        }
        int i_t = temp;
        int f_t = (temp * 100 - i_t * 100);
        if (f_t < 0) {
            f_t *= -1;
        }
        host_send_text("\tSensor[%zd] = %d.%02dC\n", sensor_id, i_t, f_t);
    }
    return 0;
}

static int enable_sensor_text(int argc, const char **argv)
{
    if (argc <= 0) {
        host_send_text("\tUsage:\napp enable <sensor_id> <sensor_id> ...\n");
        return -1;
    }
    for (int i = 0; i < argc; i++) {
        uint32_t sensor_id = atoi(argv[i]);
        if (sensor_id < 0 || sensor_id >= MAX_SENSOR_COUNT) {
            continue;
        }
        if (sensor_set_enabled(sensor_id, true) != 0) {
            host_send_text("\tFailed to enable sensor[%zd]. Please check its configuration\n",
                           sensor_id);
            continue;
        }
        else {
            host_send_text("\tSensor[%zd] enabled\n", sensor_id);
        }
        sensor_refresh_data(sensor_id);
    }
    return 0;
}

static int disable_sensor_text(int argc, const char **argv)
{
    if (argc <= 0) {
        host_send_text("\tUsage:\napp disable <sensor_id> <sensor_id> ...\n");
        return -1;
    }
    for (int i = 0; i < argc; i++) {
        uint32_t sensor_id = atoi(argv[i]);
        if (sensor_id < 0 || sensor_id >= MAX_SENSOR_COUNT) {
            continue;
        }
        sensor_set_enabled(sensor_id, false);
    }
    return 0;
}

static int config_sensor_text(int argc, const char **argv)
{
    if (argc <= 1) {
        host_send_text("\tUsage:\napp config <sensor_id> <interval> <offset> <coef>\n");
        return -1;
    }

    uint32_t sensor_id = atoi(argv[0]);
    if (sensor_id < 0 || sensor_id >= MAX_SENSOR_COUNT) {
        host_send_text("Incorrect sensor id\n");
        host_send_text("\tUsage:\napp config <sensor_id> <interval> <offset> <coef>\n");
        return -1;
    }
    if (argc > 1) {
        uint32_t interval = atoi(argv[1]);
        if (sensor_set_interval(sensor_id, interval) == 0) {
            host_send_text("Interval set to: %dms\n", interval);
        }
        else {
            host_send_text("Failed to set interval to: %dms\n", interval);
        }
    }
    if (argc > 2) {
        int32_t offset = atoi(argv[2]);
        if (sensor_set_offset(sensor_id, offset) == 0) {
            host_send_text("Calibration offset set to: %d\n", offset);
        }
        else {
            host_send_text("Failed to set calibration offset to: %d\n", offset);
        }
    }
    if (argc > 3) {
        float coef = atof(argv[3]);
        if (sensor_set_coef(sensor_id, coef) == 0) {
            host_send_text("Calibration coefficient set to: %s\n", argv[3]);
        }
        else {
            host_send_text("Failed to set calibration coefficient to: %s\n", argv[3]);
        }
    }
    sensor_refresh_data(sensor_id);
    return 0;
}

static int toggle_interface_mode_text(int argc, const char **argv)
{
    host_send_text("Interface will switch to binary mode\n");
    host_send_text("Use binary command to return back to text mode, or reset the device\n");
    host_set_mode(HOST_INTERFACE_BIN);
    return 0;
}

static int toggle_interface_mode_bin(const Request *request)
{
    Response response = Response_init_zero;
    response.status   = Status_SUCCESS;
    response.type     = RequestType_Toggle;

    uint8_t buffer[Response_size];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof buffer);

    bool status = pb_encode(&ostream, Response_fields, &response);
    if (status) {
        host_send_bin(buffer, ostream.bytes_written);
    }

    host_set_mode(HOST_INTERFACE_TEXT);
    host_send_text("Interface switched to text mode\n");
    return 0;
}

static int config_sensor_bin(const Request *request)
{
    Response response = Response_init_zero;
    response.status   = Status_SUCCESS;
    response.type     = RequestType_ConfigSensor;

    uint32_t sensor_id = request->request.config.sensor_id;

    if (sensor_id > 0 && sensor_id < MAX_SENSOR_COUNT) {
        if (request->request.config.command == ConfigSensorCommand_SetConfig) {
            if (request->request.config.has_coef) {
                sensor_set_coef(sensor_id, request->request.config.coef);
            }
            if (request->request.config.has_offset) {
                sensor_set_offset(sensor_id, request->request.config.offset);
            }
            if (request->request.config.has_interval) {
                sensor_set_interval(sensor_id, request->request.config.interval);
            }
            if (sensor_get_enabled(sensor_id)) {
                sensor_refresh_data(sensor_id);
            }
        }
        else if (request->request.config.command == ConfigSensorCommand_Enable) {
            if (sensor_set_enabled(sensor_id, true) == 0) {
                sensor_refresh_data(sensor_id);
            }
            else {
                response.status = Status_GENERAL_FAILURA;
            }
        }
        else if (request->request.config.command == ConfigSensorCommand_Disable) {
            sensor_set_enabled(sensor_id, false);
        }
        else {
            response.status = Status_GENERAL_FAILURA;
        }
    }
    else {
        response.status = Status_INVALID_SENSOR_ID;
    }

    uint8_t buffer[Response_size];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof buffer);

    bool status = pb_encode(&ostream, Response_fields, &response);
    if (status) {
        host_send_bin(buffer, ostream.bytes_written);
    }
    return 0;
}

static int read_sensor_bin(const Request *request)
{
    Response response = Response_init_zero;
    response.status   = Status_SUCCESS;
    response.type     = RequestType_ReadData;

    uint32_t sensor_id = request->request.read_data.sensor_id;

    response.type = request->type;

    if (sensor_id >= 0 && sensor_id < MAX_SENSOR_COUNT) {
        
        if (!sensor_get_enabled(sensor_id)) {
            response.status = Status_SENSOR_OFFLINE;
        }
        else {
            int32_t temp = sensor_get_temp(request->request.read_data.sensor_id);
            response.has_temperature = true;
            response.temperature.sensor_id = sensor_id;
            response.temperature.data = temp;
        }
    }
    else {
        response.status = Status_INVALID_SENSOR_ID;
    }

    uint8_t buffer[Response_size];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof buffer);

    bool status = pb_encode(&ostream, Response_fields, &response);
    if (status) {
        host_send_bin(buffer, ostream.bytes_written);
    }

    return 0;
}
