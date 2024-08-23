cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(temp_reader)

# Include nanopb module
list(APPEND CMAKE_MODULE_PATH ${ZEPHYR_BASE}/modules/nanopb)
include(nanopb)

# Include usb module
include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)

# Include protobuf message definition
zephyr_nanopb_sources(app proto/message.proto)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
