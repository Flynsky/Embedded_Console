# VCP-Console
A simple non-intrusive
Virtual Com Port Console for STM32.
```
.-')    .-') _   _   .-')
  ( OO ). (  OO) ) ( '.( OO )_
 (_)---\_)/     '._ ,--.   ,--.).-----.  .-----.  
 /    _ | |'--...__)|   `.'   |'-' _'  |'-'  |  | 
 \  :` `. '--.  .--'|         |   |_  <    .'  /  
  '..`''.)   |  |   |  |'.'|  |.-.  |  | .'  /__  
 .-._)   \   |  |   |  |   |  |\ `-'   /|       | 
 \       /   |  |   |  |   |  ' `----'' `-------' 
  `-----'    `--'   `--'   `--' `----'' `-------' 
```

# Features:
- Control STM32 via Serial Monitor
- Send Messages via printf()
- Flash Firmware
- Portable/customisable

# Setup:
## Libary
- add Libary/debugf_vcp to STM32 project

## CubeMX
- Enable USB
- Configure USB_DEVICE as VCP

## CMakeList.txt

- to enable floats add:
```
# Enable hardware floating-point support in the compiler (if applicable)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Add the linker flag to enable floating-point support in printf
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float -u _scanf_float")

# Link the math library to support floating-point operations
target_link_libraries(${PROJECT_NAME} m)

# Allow GCC extensions (binary literals, etc.)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

- to export to .bin for dfu:
# Convert output to hex and binary
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
)
```
# Convert to bin file
```
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
)
```
## dfu
- install https://dfu-util.sourceforge.net/
- install required Driver 
