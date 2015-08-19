@ECHO OFF

arm-none-eabi-gcc -O0 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -g armc-00.c -o kernel.elf
