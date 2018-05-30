#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

# Get the tutorial name from the script directory
tutorial=${scriptdir##*/}

if [ $# -ne 1 ]; then
    echo "usage: build.sh <pi-model>" >&2
    echo "       pi-model options: rpi1, rpi1bp, rpi2, rpi2bp, rpi3" >&2
    exit 1
fi

# The raspberry pi model we're targetting
model="${1}"

# Make sure we have the c compiler we need
gcc_version=$(arm-none-eabi-gcc --version)

if [ $? -ne 0 ]; then
    echo "Cannot find arm-none-eabi-gcc executable - please make sure it's present in your path" >&2
    echo "PATH: ${PATH}" >&2
    exit 1
fi

# Common CFLAGS
ccflags="-g"
ccflags="${ccflags} -nostartfiles"
ccflags="${ccflags} -mfloat-abi=hard"

# Whatever specific flags we use should also include the common c flags
cflags="${ccflags}"

# Build the model specific flags
if [ "${model}" = "rpi1" ] || [ "${model}" = "rpi1bp" ]; then
    cflags="${cflags} -DRPI1"
    cflags="${cflags} -O0"
    cflags="${cflags} -mfpu=vfp"
    cflags="${cflags} -mfloat-abi=hard"
    cflags="${cflags} -march=armv6zk"
    cflags="${cflags} -mtune=arm1176jzf-s"
    if [ "${model}" = "rpi1bp" ]; then
        cflags="${cflags} -DIOBPLUS"
    fi
elif [ "${model}" = "rpi2" ]; then
    cflags="${cflags} -DRPI2"
    cflags="${cflags} -DIOBPLUS"
    cflags="${cflags} -O0"
    cflags="${cflags} -mfpu=neon-vfpv4"
    cflags="${cflags} -mfloat-abi=hard"
    cflags="${cflags} -march=armv7-a"
    cflags="${cflags} -mtune=cortex-a7"
elif [ "${model}" = "rpi3" ]; then
    cflags="${cflags} -DRPI3"
    cflags="${cflags} -DIOBPLUS"
    cflags="${cflags} -O0"
    cflags="${cflags} -mfpu=crypto-neon-fp-armv8"
    cflags="${cflags} -mfloat-abi=hard"
    cflags="${cflags} -march=armv8-a+crc"
    cflags="${cflags} -mcpu=cortex-a53"
else
    echo "Unknown Raspberry Pi Model ${model}" >&2
    exit 1
fi

kernel_elf="${scriptdir}/kernel.${tutorial}.${model}.elf"
kernel_img="${scriptdir}/kernel.img"

arm-none-eabi-gcc ${cflags} ${scriptdir}/*.c -o ${kernel_elf}

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile!" >&2
    exit 1
fi

arm-none-eabi-objcopy ${kernel_elf} -O binary ${kernel_img}
