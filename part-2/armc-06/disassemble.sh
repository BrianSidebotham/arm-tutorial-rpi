#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
base=${scriptdir}/../..

# Get the compiler to use, etc.
. ${base}/shell/common_build.sh
. ${base}/shell/common_functions.sh


# Get the tutorial name from the script directory
tutorial=${scriptdir##*/}

for elf in ${scriptdir}/*.elf; do
    img=${elf%.elf}.img
    printf "%s\n" "Disassembling ${elf} to ${elf}.asm"
    arm-none-eabi-objdump -D ${elf} > ${elf}.asm
    arm-none-eabi-nm ${elf} > ${elf}.nm
    hexdump ${img} > ${img}.hexdump
done
