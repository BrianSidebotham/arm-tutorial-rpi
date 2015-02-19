#!/bin/sh

arm-none-eabi-gcc -DRPIBPLUS -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g armc-01.c -o kernel.elf
