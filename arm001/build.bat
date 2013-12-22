@ECHO OFF

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g armc-2.c -o kernel2.elf
arm-none-eabi-objcopy kernel2.elf -O binary kernel2.img

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g armc-3.c -o kernel3.elf
arm-none-eabi-objcopy kernel3.elf -O binary kernel3.img

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g armc-4.c -o kernel4.elf
arm-none-eabi-objcopy kernel4.elf -O binary kernel4.img

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-verbose armc-5.c -o kernel5.elf
arm-none-eabi-objcopy kernel5.elf -O binary kernel5.img

arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -g -Wl,-verbose -Wl,-T,rpi.x armc-6.c -o kernel6.elf
arm-none-eabi-objcopy kernel6.elf -O binary kernel6.img

pause
