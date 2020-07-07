#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
basedir=${scriptdir}/../..

# Get the compiler to use, etc.
. ${basedir}/shell/common_build.sh
. ${basedir}/shell/common_functions.sh


# Get the tutorial name from the script directory
tutorial=${scriptdir##*/}

if [ $# -ne 1 ]; then
    echo "usage: build.sh <pi-model>" >&2
    echo "       pi-model options: rpi0, rpi1, rpi1bp, rpi2, rpi3, rpibp" >&2
    exit 1
fi

# The raspberry pi model we're targetting
model="${1}"

# If you swap rpi throughout the build, CMake uses cache that we don't want it too.
# There's no option to tell CMake not to do this. In particular it will hold on to
# the old toolchain reference.
if [ -d ${scriptdir}/build ]; then
    rm -rf ${scriptdir}/build
fi

mkdir -p ${scriptdir}/build && cd ${scriptdir}/build
echo "TOOLCHAIN FILE ${cmake_toolchain_dir}/toolchain-arm-none-eabi-${model}.cmake"

cmake -G "CodeBlocks - Unix Makefiles" \
        -DTUTORIAL="${tutorial}" \
        -DBOARD="${model}" \
        -DTC_PATH="${tcpath}/" \
        -DCMAKE_TOOLCHAIN_FILE=${cmake_toolchain_dir}/toolchain-arm-none-eabi-${model}.cmake \
        ${scriptdir}


if [ $? -ne 0 ]; then
    echo "Failed to configure!" >&2
    exit 1
fi

make VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to build!" >&2
    exit 1
fi

# Generate a new card image
${basedir}/card/make_card.sh ${model} ${tutorial} ${scriptdir}/build/kernel.${tutorial}.${model}.img

exit 0
