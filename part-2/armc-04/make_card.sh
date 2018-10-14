#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
base=${scriptdir}/../..

# Get the compiler to use, etc.
. ${base}/shell/common_build.sh
. ${base}/shell/common_functions.sh


# Get the tutorial name from the script directory
tutorial=${scriptdir##*/}

if [ $# -ne 1 ]; then
    echo "usage: make_card.sh <pi-model>" >&2
    echo "       pi-model options: rpi0, rpi1, rpi1bp, rpi2, rpi3, rpibp" >&2
    exit 1
fi

# The raspberry pi model we're targetting
model="${1}"

kernel_file=kernel.${tutorial}.${model}.img
disk_image=card.${tutorial}.${model}.img

if [ ! -f "${scriptdir}/${kernel_file}" ]; then
    echo "Build the kernel file first before trying to make a card with it" >&2
    exit 1
fi

sudo ${scriptdir}/../../card/create_card_image.sh \
    "${scriptdir}/${kernel_file}" \
    "${scriptdir}/${disk_image}"

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to create card image!" >&2
    exit 1
fi
