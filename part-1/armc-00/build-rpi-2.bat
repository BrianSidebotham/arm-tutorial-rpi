@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7-a -mtune=cortex-a7 -g armc-00.c -o kernel.elf
