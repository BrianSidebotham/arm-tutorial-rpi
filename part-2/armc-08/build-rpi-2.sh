#!/bin/sh

arm-none-eabi-gcc -O0 -DRPI2 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7-a -mtune=cortex-a7 -nostartfiles -g -Wl,-T,rpi.x armc-08.c armc-08-cstartup.c armc-08-start.S -o kernel.elf
arm-none-eabi-objcopy kernel.elf -O binary kernel.img
