
#   Part of the Raspberry-Pi Bare Metal Tutorials
#   Copyright (c) 2013-2015, Brian Sidebotham
#   All rights reserved.

#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are met:

#   1. Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.

#   2. Redistributions in binary form must reproduce the above copyright notice,
#       this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.

#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#   ARE DISCLAIMED. IN NO EVENT SHALL THE
#   Part of the Raspberry-Pi Bare Metal Tutorials
#   https://www.valvers.com/rpi/bare-metal/
#   Copyright (c) 2013, Brian Sidebotham

#   This software is licensed under the MIT License.
#   Please see the LICENSE file included with this software.

# CMake build environment for the Valvers Raspberry-Pi bare metal tutorials

# CMake 2.8.10 Documentation: http://www.cmake.org/cmake/help/v2.8.10/cmake.html

cmake_minimum_required( VERSION 2.8 )

# Mark the language as C so that CMake doesn't try to test the C++
# cross-compiler's ability to compile a simple program because that will fail
project( valvers-${TUTORIAL} C ASM )

# We had to adjust the CMAKE_C_FLAGS variable in the toolchain file to make sure
# the compiler would work with CMake's simple program compilation test. So unset
# it explicitly before re-setting it correctly for our system
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles" )

add_executable( kernel.${TUTORIAL}.${BOARD}
    ${TUTORIAL}.c
    ${TUTORIAL}-cstartup.c
    ${TUTORIAL}-cstubs.c
    ${TUTORIAL}-start.S
    rpi-armtimer.c rpi-armtimer.h
    rpi-base.h
    rpi-gpio.c rpi-gpio.h
    rpi-interrupts.c rpi-interrupts.h
    rpi-systimer.c rpi-systimer.h )

add_custom_command(
    TARGET kernel.${TUTORIAL}.${BOARD} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ./kernel.${TUTORIAL}.${BOARD}${CMAKE_EXECUTABLE_SUFFIX} -O binary ./kernel.${TUTORIAL}.${BOARD}.img
    COMMAND ${CMAKE_OBJDUMP} -l -S -D ./kernel.${TUTORIAL}.${BOARD}${CMAKE_EXECUTABLE_SUFFIX} > ./kernel.${TUTORIAL}.${BOARD}.asm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Convert the ELF output file to a binary image" )
