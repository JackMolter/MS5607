cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(MS5607)

pico_sdk_init()

add_executable(barometer
    barometer.c
    )

target_link_libraries(barometer
    pico_stdlib
    hardware_i2c

)

pico_enable_stdio_usb(barometer 1)

pico_add_extra_outputs(barometer)