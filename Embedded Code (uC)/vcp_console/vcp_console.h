/**
 * Adds debugf for STM32.
 *
 * Add to CMakeList.txt:

# Enable hardware floating-point support in the compiler (if applicable)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Add the linker flag to enable floating-point support in printf
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float -u _scanf_float")


# Link the math library to support floating-point operations
target_link_libraries(${PROJECT_NAME} m)

# Allow GCC extensions (binary literals, etc.)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

* Change:

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    debugf_vcp/debugf_vcp.c
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    debugf_vcp
    # Add user defined include paths
)
*

add to Src\usbd_cdc_if.C
#include "debugf_vcp.h"

 */

#ifndef DEBUGF_VCP_H
#define DEBUGF_VCP_H

// #include <stdint.h>      //includes datatypes uint32_t
#include <stdarg.h> //used for debugf
#include <stdio.h>  //tf i know
#include <string.h> //string operators to make my live easier
void debugf(const char *__restrict format, ...);

void console_check(); // checks if new command is recieved
void print_startup();
void jump_to_dfu_bootloader();

#endif