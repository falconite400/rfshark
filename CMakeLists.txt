cmake_minimum_required(VERSION 3.30)
set(CMAKE_C_STANDARD 23)

set(picotool_DIR ~/picotool)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(PICO_SDK_PATH ~/pico-sdk)
set(PICOTOOL_FETCH_FROM_GIT_PATH ~/picotool)
set(PICO_BOARD pico_w)

# pull in pico sdk
include(pico_sdk_import.cmake)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# initialize the sdk
pico_sdk_init()

project(pico-radio C CXX ASM)

add_library(cc1101)
add_subdirectory(./cc1101)

# declare that we are building a program called main 
add_executable(main main.c)

target_link_libraries(main
    pico_stdlib
    pico_cyw43_arch_none
    hardware_spi
    cc1101
)

# declare that also want extra build formats like .uf2 
# which can be dragged  and dropped onto pico over usb
pico_add_extra_outputs(main)

# enable 
pico_enable_stdio_usb(main 1)