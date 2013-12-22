@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g armc-002.c -o kernel2.elf
arm-none-eabi-objcopy kernel2.elf -O binary kernel2.img

pause
