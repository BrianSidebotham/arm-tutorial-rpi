@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-T,rpi.x armc-007.c -o kernel-007.elf
arm-none-eabi-objcopy kernel-007.elf -O binary kernel-007.img

pause
