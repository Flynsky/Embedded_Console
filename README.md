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
- Flash Firmware via dfu
- Portable/customisable

# Setup:

### C Source Code
- add folder "./Embedded Code(uC)/vcp_console" to STM32 project

### CubeMX
- Enable USB
- Configure USB_DEVICE as VCP

### CMakeList.txt
- add vcp_console 
```
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    vcp_console/vcp_console.c
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    vcp_console
    # Add user defined include paths
)
```

- to enable floats in printf add:
```
# Enable hardware floating-point support in the compiler (if applicable)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Add the linker flag to enable floating-point support in printf
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float -u _scanf_float")

# Link the math library to support floating-point operations
target_link_libraries(${PROJECT_NAME} m)

# Allow GCC extensions (binary literals, etc.)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
```
- to export to .bin for dfu:
```
### Convert to bin file
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
)
```
## Device Firmware Update dfu
- install/download dfu-util from https://dfu-util.sourceforge.net/
- install required Driver (s. dfu-util website)

# Python Console
- Install Python
- Install packages struct, threading, pyserial, subprocess

# First Steps
- Flash the console to the microcontroller (s. Setup)
- Connect microcontroller via USB Cable to Computer
- Run Console ./Python Console (PC)/console.py. Should autoconnect.
  If not: check errors. 
	  use /l for list of connected Devices, 
	  if y found yours, use /s number to connect
- use /t string to send strings to your microcontroller
