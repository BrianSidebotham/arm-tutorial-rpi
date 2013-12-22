@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-verbose -Wl,-T,rpi.x armc-006.c -o kernel-006.elf
arm-none-eabi-objcopy kernel-006.elf -O binary kernel-006.img

pause
