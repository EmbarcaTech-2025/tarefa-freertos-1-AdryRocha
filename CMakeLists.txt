cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Bloco de configuração do VS Code (manter) ---
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# --- Fim do Bloco ---

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(rtos_bitdoglab C CXX ASM)
pico_sdk_init()

# --- Configuração Manual Completa do FreeRTOS ---

set(FREERTOS_PORT GCC_RP2040 CACHE STRING "FreeRTOS port for RP2040")

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(free_rtos_kernel)

add_executable(rtos_bitdoglab
    src/main.c
    src/led_rgb.c
    src/buzzer.c
    src/button.c
    ${CMAKE_CURRENT_LIST_DIR}/free_rtos_kernel/portable/MemMang/heap_4.c
)

target_include_directories(rtos_bitdoglab PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/free_rtos_kernel/include
    ${CMAKE_CURRENT_LIST_DIR}/free_rtos_kernel/portable/GCC/ARM_CM0
)

# CORREÇÃO: Removida a biblioteca "pico_cyw43_arch_nonos_poll" que não é necessária.
target_link_libraries(rtos_bitdoglab
    pico_stdlib
    hardware_gpio
    hardware_pwm
    freertos_kernel
    freertos_config
)

# --- Fim da Configuração ---

pico_set_program_name(rtos_bitdoglab "RTOS BitDogLab")
pico_set_program_version(rtos_bitdoglab "0.1")

pico_enable_stdio_usb(rtos_bitdoglab 1)
pico_enable_stdio_uart(rtos_bitdoglab 0)

pico_add_extra_outputs(rtos_bitdoglab)
