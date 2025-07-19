# Embedded_Console
## Features:
- Control Arduino, STM32 or any other C++ microcontroller via Sting-Based Commands
- Send Messages via printf()
- Flash Firmware via dfu
- Portable/customisable

## Overview
![Diagram](./doc/pic/overview_uml.svg)

## Getting Started
<details>
	
### Adding Microcontroller Firmware
- Copy or include ./firmware-mcu/ in your project.
- Choose with what hardware y wanna acess the console. UART? USB? Serial?
- Choose an exisitng Stream object or create your own by inheriting from <<interface>> IStream.h. 
- create a console object with given stream.
```Cpp
#include "Console.h"
#include "StreamStmUSB.h"
StreamStmUSB streamUSB;
Console console(streamUSB);
```
- rework bool Console::recieveCommands() in /src/Console.cpp to fit your needs.
- call console.recieveCommands() periodicly in your code (as quick as feasable).
```Cpp
while(1){
console.recieveCommands();
HAL_delay(10):
}
```
### First bootup
- Flash the microcontroller with console (s. Setup)
- Connect microcontroller via USB Cable to Computer
- Run Console ./Python Console (PC)/console.py. Should autoconnect.
  If not: check errors. 
	  use /l for list of connected Devices, 
	  if y found yours, use /s number to connect
- use /t string to send strings to your microcontroller

</details>

## Complete Project Setup for STM32
<details>
	
### 1. CubeMX
- Enable USB
- Configure USB_DEVICE as VCP
- Compile as CMake Project

### 2. Change CMake projet form C to Cpp
- rename main.c to main.cpp,
- in *cmake\stm32cubemx\CMakeLists.txt* change the file name main.c to main.cpp
- in *.\CMakeLists.txt* add/change:
```CMake
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ./wherever y like it/Console.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ./Embedded_Console/firmware-mcu/include
)
```

### 3. CMakeList.txt
- add C++ as a language to CMakeLists.txt:
```CMake
enable_language(C CXX ASM)
```
- add libary: 

```CMake
add_subdirectory(./Embedded_Console/firmware-mcu/)
target_link_libraries(${CMAKE_PROJECT_NAME}
    embedded_console
)
```

- to enable floats in printf add:
```CMake
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
```CMake
# to export to .bin for dfu:
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
)
# post-build automatic upload
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Flashing via DFU..."
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dfu-util-static.exe
            -a 0 -i 0 -s 0x08000000:leave
            -D ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    COMMENT "Uploading firmware using dfu-util (live output)"
    VERBATIM
)
```

## 4. Device Firmware Update via dfu-util
- install/download dfu-util from https://dfu-util.sourceforge.net/
- Windows only: install required Driver (s. dfu-util website)
- flash with
```
.\dfu-util-static.exe -a 0 -i 0 -s 0x08000000:leave -D .\build\Debug\ProjekName.bin
```
- optional: change suffix(validate firmware)
```
.\dfu-suffix.exe -v 0x0483 -p 0xdf11 -d 0x2200 -a .\build\Debug\stm32f401ConsoleTest.bin 
```
</details>

# Python Console
- Install Python
- Install packages struct, threading, pyserial, subprocess

