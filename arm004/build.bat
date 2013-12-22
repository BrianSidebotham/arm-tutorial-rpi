@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g armc-004.c -o kernel4.elf
arm-none-eabi-objcopy kernel4.elf -O binary kernel4.img

pause
