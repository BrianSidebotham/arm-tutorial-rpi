#!/bin/sh

arm-none-eabi-gcc -O2 -DRPI2 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7-a -nostartfiles -g armc-03.c -o kernel.elf
arm-none-eabi-objcopy kernel.elf -O binary kernel.img
