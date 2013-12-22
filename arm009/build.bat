@ECHO OFF

arm-none-eabi-gcc -O0 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-T,rpi.x -o kernel-009.elf armc-009.c armc-009-cstartup.c armc-009-start.S armc-009-cstubs.c

arm-none-eabi-objcopy kernel-009.elf -O binary kernel-009.img

pause
