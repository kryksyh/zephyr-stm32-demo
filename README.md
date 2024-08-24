![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/dmtr-makarenko/temp-sensor/build.yml?logo=github)
[![GitHub License](https://img.shields.io/github/license/dmtr-makarenko/temp-sensor?cacheSeconds=1)](https://github.com/dmtr-makarenko/temp-sensor/blob/main/LICENSE)
[![Version](https://img.shields.io/github/v/release/dmtr-makarenko/temp-sensor)](https://github.com/dmtr-makarenko/temp-sensor/releases/tag/v1.0.0)

# Temperature Reader
 
## Overview

**Temperature Reader** is a sample Zephyr application that simulates reading temperatures and provides a basic serial interface for communicating with the device.

## Prerequisites

Before you start building and running the application, make sure you have set up your development environment with all the required tools and dependencies for Zephyr. You also need to add **nanopb** to your workspace.

To add **nanopb** to your workspace, run these commands:

```bash
west config manifest.project-filter -- +nanopb
west update
```

## Build
To build the application for a supported Zephyr board:

Make sure your Zephyr environment is set up correctly.
Run this command, replacing `<board_name>` with your target board (for example, nucleo_f207zg):

```bash
west build -b <board_name> path/to/the/repo
```

Replace `path/to/the/repo` with the folder where the application is located.

> [!TIP]
> If you have any issues during the build, please consult with [the build workflow](../../actions/workflows/build.yml) for this repo

## Usage
The application supports two modes: text mode and binary mode. In text mode, you can interact with the device using simple commands. In binary mode, you use Protocol Buffers for more complex communication.

### Text Mode
To use the device in text mode, you need a terminal application like `minicom`. Follow these steps:

1. Connect to the device using minicom:

```
minicom -c on -D /dev/ttyACM0
```

2. Use these commands to interact with the device:

   - help: Shows a list of available commands.
   - app read <sensor_id>: Reads data from a sensor.
   - app config <id> <interval> <offset> <coefficient>: Configures a sensor with specific settings.
   - app enable <sensor_id>: Turns on the specified sensor.
   - app toggle: Switches the device to binary mode.

3. Follow the instructions on the screen to read and set up sensor data.

#### Text mode demo

 [Text mode demo](https://github.com/user-attachments/assets/4941b535-587a-4116-855b-a127e266bf0f)


### Binary Mode
To use the device in binary mode, you send and receive messages using protobuf. Here’s how:

1. Use `protoc` with the message.proto file to encode commands and send them to the device.

2. Example commands:

- To read temperature from sensor 0:

```bash
echo -e "type: ReadData read_data:{sensor_id: 0}" | protoc --encode=Request message.proto > /dev/ttyACM0
```
- To get the response:

```
dd if=/dev/ttyACM0 bs=1 count=9 status=none | protoc --decode Response message.proto
```
> [!IMPORTANT]
> Please note that due to this approach's limitations, you must know the amount of data to read beforehand (note `count=9` above), greatly impacting its usefulness.

#### Binary mode demo

[Binary mode demo](https://github.com/user-attachments/assets/bc89f059-9aa4-408d-8ec4-8e7c44918eda)
