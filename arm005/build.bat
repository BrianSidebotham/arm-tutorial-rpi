@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-verbose armc-005.c -o kernel5.elf
arm-none-eabi-objcopy kernel5.elf -O binary kernel5.img

pause
