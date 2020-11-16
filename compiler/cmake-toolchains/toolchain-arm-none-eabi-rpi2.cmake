
#   Part of the Raspberry-Pi Bare Metal Tutorials
#   https://www.valvers.com/rpi/bare-metal/
#   Copyright (c) 2013, Brian Sidebotham

#   This software is licensed under the MIT License.
#   Please see the LICENSE file included with this software.

# A CMake toolchain file so we can cross-compile for the Rapsberry-Pi bare-metal

# usage
# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-none-eabi.cmake ../

# The Generic system name is used for embedded targets (targets without OS) in
# CMake
set( CMAKE_SYSTEM_NAME          Generic )
set( CMAKE_SYSTEM_PROCESSOR     BCM2836 )

# Set a toolchain path. You only need to set this if the toolchain isn't in
# your system path.
IF( $TC_PATH )
STRING(APPEND TC_PATH "/")
ENDIF()

# The toolchain prefix for all toolchain executables
set( CROSS_COMPILE arm-none-eabi- )

# specify the cross compiler. We force the compiler so that CMake doesn't
# attempt to build a simple test program as this will fail without us using
# the -nostartfiles option on the command line

set( CMAKE_C_COMPILER "${TC_PATH}${CROSS_COMPILE}gcc" )
set( CMAKE_ASM_COMPILER "${TC_PATH}${CROSS_COMPILE}gcc" )

# Because the cross-compiler cannot directly generate a binary without complaining, just test
# compiling a static library instead of an executable program
set( CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY )

# We must set the OBJCOPY setting into cache so that it's available to the
# whole project. Otherwise, this does not get set into the CACHE and therefore
# the build doesn't know what the OBJCOPY filepath is
set( CMAKE_OBJCOPY      ${TC_PATH}${CROSS_COMPILE}objcopy
    CACHE FILEPATH "The toolchain objcopy command " FORCE )

set( CMAKE_OBJDUMP      ${TC_PATH}${CROSS_COMPILE}objdump
CACHE FILEPATH "The toolchain objdump command " FORCE )


# Set the CMAKE C flags (which should also be used by the assembler!
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon-vfpv4" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=cortex-a7" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )

# Add the raspberry-pi 2 definition so conditional compilation works
add_definitions( -DRPI2=1 )
set( BOARD rpi2 )
