
#   Part of the Raspberry-Pi Bare Metal Tutorials
#   https://www.valvers.com/rpi/bare-metal/
#   Copyright (c) 2013, Brian Sidebotham

#   This software is licensed under the MIT License.
#   Please see the LICENSE file included with this software.

# CMake build environment for the Valvers Raspberry-Pi bare metal tutorials

# CMake 2.8.10 Documentation: http://www.cmake.org/cmake/help/v2.8.10/cmake.html

cmake_minimum_required( VERSION 2.8 )

# Set the tutorial number so it's easier to re-use the CMakeLists.txt
set( TUT 010 )

# Mark the language as C so that CMake doesn't try to test the C++
# cross-compiler's ability to compile a simple program because that will fail
project( armc-${TUT}-tutorial C ASM )

# We had to adjust the CMAKE_C_FLAGS variable in the toolchain file to make sure
# the compiler would work with CMake's simple program compilation test. So unset
# it explicitly before re-setting it correctly for our system
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles" )

add_executable( armc-010-${BOARD}
    armc-010.c
    armc-010-cstartup.c
    armc-010-cstubs.c
    armc-010-start.S )

add_custom_command(
    TARGET armc-010-${BOARD} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ./armc-${TUT}-${BOARD}${CMAKE_EXECUTABLE_SUFFIX} -O binary ./kernel-${BOARD}.img
    COMMAND ${CMAKE_OBJDUMP} -l -S -D ./armc-${TUT}-${BOARD}${CMAKE_EXECUTABLE_SUFFIX} > ./kernel-${BOARD}.img.asm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Convert the ELF output file to a binary image" )
