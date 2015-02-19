@ECHO OFF

arm-none-eabi-gcc -O2 -DRPI2 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7-a -mtune=cortex-a7 -nostartfiles -g armc-01.c -o kernel.elf
