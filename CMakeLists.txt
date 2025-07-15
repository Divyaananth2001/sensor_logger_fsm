cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(SensorLoggerFSM LANGUAGES C CXX)

add_definitions(-D_CRT_SECURE_NO_WARNINGS -D__WINDOWS__ -D_WIN32)

include_directories(
    config
    FreeRTOS-Kernel/include
    FreeRTOS-Kernel/portable/MSVC-MingW
    FreeRTOS-Plus-CLI
)

file(GLOB KERNEL_SRC
    "FreeRTOS-Kernel/*.c"
    "FreeRTOS-Kernel/portable/MSVC-MingW/*.c"
    "FreeRTOS-Kernel/portable/MemMang/heap_4.c"
    "FreeRTOS-Plus-CLI/FreeRTOS_CLI.c"
)

set(SRC_FILES
    src/main.cpp
    src/sensor.cpp
    src/sensor.hpp
    src/logger.cpp
    src/logger.hpp
    src/fsm.cpp
    src/fsm.hpp
    src/cli_commands.cpp
    src/cli_commands.hpp
)

add_executable(sensor_logger
                ${SRC_FILES}
                ${KERNEL_SRC}
)