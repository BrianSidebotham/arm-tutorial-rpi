#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
base=${scriptdir}/../..

# Get the compiler to use, etc.
. ${base}/shell/common_build.sh
. ${base}/shell/common_functions.sh


# Get the tutorial name from the script directory
tutorial=${scriptdir##*/}

if [ $# -ne 1 ]; then
    echo "usage: build.sh <pi-model>" >&2
    echo "       pi-model options: rpi0, rpi1, rpi1bp, rpi2, rpi3, rpibp" >&2
    exit 1
fi

# The raspberry pi model we're targetting
model="${1}"

disk_name=disk-${model}.img

# Make sure we have the c compiler we need
gcc_version=$(${toolchain}gcc --version)

if [ $? -ne 0 ]; then
    echo "Cannot find ${toolchain}gcc executable - please make sure it's present in your path" >&2
    echo "PATH: ${PATH}" >&2
    exit 1
fi

# Common CFLAGS
ccflags="-g"
ccflags="${ccflags} -nostartfiles"
ccflags="${ccflags} -mfloat-abi=hard"
ccflags="${ccflags} -O0"

# Whatever specific flags we use should also include the common c flags
cflags="${ccflags}"

# Determine if the model is a b+ model or not
# We need to know if the raspberry pi model is b+ because the early RPI1 models have a different IO
# pin layout between the original units and the later B+ units. Similarly the RPI3 models have a
# different IO arrangement on the 3B+ units compared to the original 3B. We need to be able to
# be able to adjust the code for whichever model we're targetting
case "${model}" in
    *bp) cflags="${cflags} -DIOBPLUS" ;;
esac

case "${model}" in
    rpi0*)
        cflags="${cflags} -DRPI0"
        cflags="${cflags} -mfpu=vfp"
        cflags="${cflags} -march=armv6zk"
        cflags="${cflags} -mtune=arm1176jzf-s"
        ;;
    rpi1*)
        cflags="${cflags} -DRPI1"
        cflags="${cflags} -mfpu=vfp"
        cflags="${cflags} -march=armv6zk"
        cflags="${cflags} -mtune=arm1176jzf-s"
        ;;

    rpi2*)
        cflags="${cflags} -DRPI2"
        cflags="${cflags} -mfpu=neon-vfpv4"
        cflags="${cflags} -march=armv7-a"
        cflags="${cflags} -mtune=cortex-a7"
        ;;

    rpi3*) cflags="${cflags} -DRPI3"
        cflags="${cflags} -mfpu=crypto-neon-fp-armv8"
        cflags="${cflags} -march=armv8-a+crc"
        cflags="${cflags} -mcpu=cortex-a53"
        ;;

    *) echo "Unknown model type ${model}" >&2 && exit 1
        ;;
esac

kernel_elf="${scriptdir}/kernel.${tutorial}.${model}.elf"
kernel_img="${scriptdir}/kernel.${tutorial}.${model}.img"

printf "%s\n" "${toolchain}gcc ${cflags} ${scriptdir}/*.c -o ${kernel_elf}"
${toolchain}gcc ${cflags} ${scriptdir}/*.c -o ${kernel_elf}

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to compile!" >&2
    exit 1
fi

printf "%s\n" "${toolchain}objcopy ${kernel_elf} -O binary ${kernel_img}"
${toolchain}objcopy ${kernel_elf} -O binary ${kernel_img}
