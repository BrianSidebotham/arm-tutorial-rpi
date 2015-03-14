#!/bin/sh

arm-none-eabi-gcc -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-verbose armc-05.c -o kernel.elf
arm-none-eabi-objcopy kernel.elf -O binary kernel.img
