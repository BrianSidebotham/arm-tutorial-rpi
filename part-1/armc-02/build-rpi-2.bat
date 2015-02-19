@ECHO OFF

arm-none-eabi-gcc -O2 -DRPI2 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7-a -nostartfiles -g armc-02.c -o kernel.elf
