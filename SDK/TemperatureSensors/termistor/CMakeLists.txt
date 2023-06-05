cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(termistor_project)

pico_sdk_init()

add_executable(termistor
    termistor_sdk.c
)


target_link_libraries(termistor PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
)

pico_enable_stdio_usb(termistor 1)
pico_enable_stdio_uart(termistor 0)

pico_add_extra_outputs(termistor)
