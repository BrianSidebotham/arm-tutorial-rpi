@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g armc-003.c -o kernel3.elf
arm-none-eabi-objcopy kernel3.elf -O binary kernel3.img

pause
