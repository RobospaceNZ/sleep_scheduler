cmake_minimum_required(VERSION 3.20.0)

# Use the sleep_scheduler in this repository as a Zephyr module
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sleep_scheduler_test)

target_sources(app PRIVATE src/main.c)
