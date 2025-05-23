# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Ohmimetro C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Ohmimetro 
        Ohmimetro.c 
        lib/ssd1306.c
        )

target_compile_definitions(${PROJECT_NAME} PRIVATE 
        PICO_PRINTF_SUPPORT_FLOAT=1 
        PICO_STDIO_ENABLE_PRINTF=1
)

pico_set_program_name(Ohmimetro "Ohmimetro")
pico_set_program_version(Ohmimetro "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Ohmimetro 0)
pico_enable_stdio_usb(Ohmimetro 1)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(Ohmimetro 
    ${CMAKE_CURRENT_LIST_DIR}/lib/ws2812.pio OUTPUT_DIR 
        ${CMAKE_CURRENT_LIST_DIR}/generated)

# Add the standard library to the build
target_link_libraries(Ohmimetro
    pico_stdlib 
    hardware_pio
    hardware_i2c
    hardware_adc
)

# Add the standard include files to the build
target_include_directories(Ohmimetro PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(Ohmimetro)

