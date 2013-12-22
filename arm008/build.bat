@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-T,rpi.x -o kernel-008.elf armc-008.c armc-008-cstartup.c armc-008-start.S
arm-none-eabi-objcopy kernel-008.elf -O binary kernel-008.img

pause
