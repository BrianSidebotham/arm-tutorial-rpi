#!/bin/sh

# Use an include guard
if [ "${__valvers_arm_rpi_common_build}X" != "X" ]; then
    return
fi

__valvers_arm_rpi_common_build="included"

# Which compiler are we going to use
compiler_config_file=${scriptdir}/../../compiler/.compiler_config
if [ -f ${compiler_config_file} ]; then
    . ${compiler_config_file}
else
    # Drop back to hoping the compiler is in the PATH
    toolchain=arm-none-eabi-
fi
