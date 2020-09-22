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

if [ -d ${scriptdir}/build ]; then
    rm -rf ${scriptdir}/build
    if [ $? -ne 0 ]; then
        echo "Could not remove old build directory. CMake caching requires us to in order to have a good build" >&2
        exit 1
    fi
fi

mkdir -p ${scriptdir}/build && cd ${scriptdir}/build

toolpath="${tcpath}"
if [ "${toolpath}X" != "X" ]; then
  toolpath="${toolpath}/"
fi

cmake -G "CodeBlocks - Unix Makefiles" \
        -DTUTORIAL="${tutorial}" \
        -DTC_PATH="${toolpath}" \
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
