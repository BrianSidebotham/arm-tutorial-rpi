---
layout: post
title: Step01 - Bare Metal Programming in C Pt1
---

Although the Raspberry-Pi comes with a good Linux distribution, the Pi is about software development, and sometimes we want a real-time system without an operating system. I decided it'd be great to do a tutorial outside of Linux to get to the resources of this great piece of hardware in a similar vein to the [Cambridge University Tutorials](http://www.cl.cam.ac.uk/freshers/raspberrypi/tutorials/) which are excellently written. However, they don't create an OS as purported and they start from assembler rather than C. I will simply mimic their tutorial here, but using C instead of assembler. The C compiler simply converts C syntax to assembler and then assembles this into executable code for us anyway.

NOTE: I highly recommend going through the Cambridge University Raspberry Pi tutorials as they are excellent. If you want to learn a bit of assembler too, then definitely head off to there! These pages provide a similar experience, but with the additional of writing code in C and understanding the process behind that.

## Cross -Compiling for the Raspberry Pi (BCM2835)

The [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded) project on Launchpad gives us a GCC toolchain to use for ARM compilation on either Windows or Linux. I suggest you install it (on Windows I would install it somewhere without spaces in the file path) so that you have it in your path. You should be able to type arm-none-eabi-gcc on the command line and get a response like the following:

    arm-none-eabi-gcc
    arm-none-eabi-gcc: fatal error: no input file
    compilation terminated.


This [eLinux page](http://elinux.org/RPi_Software#ARM) gives us the optimal GCC settings for compiling code for the Raspberry-Pi: `-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s`

It is noted that `-Ofast` may cause problems with some compilations, so it is probably better that we stick with the more traditional `-O2` optimisation setting. The other flags merely tell GCC what type of floating point unit we have, tell it to produce hard-floating point code (GCC can create software floating point support instead), and tells GCC what ARM processor architecture we have so that it can produce optimal and compatible assembly/machine code.

## Getting to know the Compiler and Linker

In order to use a C compiler, we need to understand what the compiler does and what the linker does in order to generate executable code. The compiler converts C statements into assembler and performs optimisation of the assembly instructions. This is in-fact all the C compiler does! The C compiler then implicitly calls the assembler to assemble that file (usually a temporary) into an object file. This will have relocatable machine code in it along with symbol information for the linker to use.

The linker's job is to link everything into an executable file. The linker requires a linker script. The linker script tells the linker how to organise the various object files. The linker will resolve symbols to addresses when it has arranged all the objects according to the rules in the linker script.

What we're getting close to here is that a C program isn't just the code we type. There are some fundamental things that must happen for C code to run. For example, some variables need to be initialised to certain values, and some variables need to be initialised to 0. This is all taken care of by an object file which is usually implicitly linked in by the linker because the linker script will include a reference to it. The object file is called crt0.o (C Run-Time 0)

This code uses symbols that the linker can resolve to clear the start of the area where initialised variables starts and ends in order to zero this memory section. It generally sets up a stack pointer, and it always includes a call to _main. Here's an important note: symbols present in C code get prepended with an underscore in the generation of the assembler version of the code. So where the start of a C program is the main symbol, in assembler we need to refer to it as it's assembler version which is _main.

Let's have a look at compiling one of the simplest programs that we can. Lets compile and link the following program:

    int main(void)
    {
        while(1)
        {
    
        }

        return 0;
    }

The command we use to compile and link is simply:

    arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s arm-test.c

GCC does successfully compile the source code (there are no C errors in it), but the linker fails with the following message:

    ../arm-none-eabi/lib/fpu\libc.a(lib_a-exit.o): In function `exit':
    exit.c:(.text.exit+0x2c): undefined reference to `_exit'
    collect2.exe: error: ld returned 1 exit status

So with our one-line command above we're invoking the C compiler, the assembler and the linker. The C compiler does most of the menial tasks for us to make life easier for us, but because we're embedded engineers (aren't we?) we MUST be aware of how the compiler, assembler and linker work at a very low level as we generally work with custom systems which we must describe intimately to the toolchain.

So there's a missing _exit symbol. This symbol is reference by the C library we're using. It is in-fact a system call. It's designed to be implemented by the OS. It would be called when a program terminates. In our case, we are our own OS at we're the only thing running, and in fact we will never exit so we do not need to really worry about it. System calls can be blank, they just merely need to be provided in order for the linker to resolve the symbol.

So the C library has a requirement of system calls. Sometimes these are already implemented as blank functions, or implemented for fixed functionality. For a list of system calls see the [newlib documentation on system calls](http://sourceware.org/newlib/libc.html#Stubs). Newlib is an open source, and lightweight C library.

The C library is what provides all of the C functionality found in standard C header files such as stdio.h, stlib.h, string.h, etc.

At this point I want to note that the standard Hello World example won't work here without an OS, and it is exactly unimplemented system calls that prevent it from being our first example. The lowest part of printf(...) includes a write function "write" - this function is used by all of the functions in the C library that need to write to a file. In the case of printf, it needs to write to the file stdout. Generally when an OS is running stdout produces output visible on a screen which can then be piped to another file system file by the OS. Without an OS, stdout generally prints to a UART to so that you can see program output on a remote screen such as a PC running a terminal program. We will discuss write implementations later on in the tutorial series, let's move on...

The easiest way to fix the link problem is to provide a minimal exit function to satisfy the linker. As it is never going to be used, all we need to do is shut the linker up and let it resolve _exit. So now we can compile the next version of the code:
    
    int main(void)
    {
        while(1)
        {
    
        }
    
        return 0;
    }

    void exit(int code)
    {
        while(1)
            ;
    }

It's important to have an infinite loop in the exit function. In the C library, which is not intended to be used with an operating system (hence arm-NONE-eabi-*), _exit is marked as being noreturn. We must make sure it doesn't return otherwise we will get a warning about it. The prototype for _exit always includes an exit code int too.

Now using the same build command above we get a clean build! Yay! But there is really a problem, in order to provide a system underneath the C library we will have to provide linker scripts and our own C Startup code. In order to skip that initially and to simply get up and running we'll just use GCC's option not to include any of the C startup routines, which excludes the need for exit too: `-nostartfiles`

## Getting to Know the Processor

As in the Cambridge tutorials we will copy their initial example of illuminating an LED in order to know that our code is running correctly.

### Raspberry-Pi Boot Process

First, let's have a look at how a Raspberry-Pi processor boots. The BCM2385 from Broadcom includes two processors that we should know about, one is a Videocore(tm) GPU which is why the Raspberry-Pi makes such a good media-centre and the other is the ARM Cortex-A8 which runs the operating system. Both of these processors share the peripheral bus and also have to share some interrupt resources. Although in this case, share means that some interrupt sources are not available to the ARM processor because they are already take by the GPU.

The GPU starts running at reset or power on and includes code to read the first FAT partition of the SD Card on the MMC bus. It searches for and loads a file called bootcode.bin into memory and starts execution of that code. The bootcode.bin bootloader in turn searches the SD card for a config.txt file to set various kernel settings before searching the SD card again for a kernel.img file which it then loads into memory at a specific address (0x8000) and starts the ARM processor executing at that memory location. The GPU is now up and running and the ARM will start to come up using the code contained in kernel.img.

Therefore in order to boot your own code, you need to firstly compile your code to an executable and name it kernel.img, and put it onto a FAT formatted SD Card, which has the GPU bootloader (bootcode.bin) on it as well. The latest Raspberry-Pi firmware is available on GitHub at: [https://github.com/raspberrypi/firmware](https://github.com/raspberrypi/firmware). The bootloader is located under the boot sub-directory: [https://github.com/raspberrypi/firmware/tree/master/boot](https://github.com/raspberrypi/firmware/tree/master/boot). The rest of the firmware provided is closed-binary video drivers. They are compiled for use under Linux so that accelerated graphics drivers are available. As we're not using Linux these files are of no use to us, only the bootloader firmware is.

All this means that the processor is already up and running when it starts to run our code. Clock sources and PLL settings are already decided and programmed in the bootloader which alleviates that problem from us. We get to just start messing with the devices registers from an already running core. This is something I'm not that used too, normally the first thing in my code would be setting up correct clock and PLL settings to initialise the processor, but the GPU has setup the basic clocking scheme for us.

The first thing we will need to set up is the GPIO controller. There are no drivers we can rely on as there is no OS running, all the bootloader has done is boot the processor into a working state, ready to start loading the OS.

You'll need to get the [Raspberry-Pi BCM2835 peripherals datahsheet](http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf) which gives us the information we require to control the IO peripherals of the BCM2835. I'll guide us through using the GPIO peripheral - there are as always some gotcha's:

We'll be using the GPIO peripheral, and it would therefore be natural to jump straight to that documentation and start writing code, but we need to first read some of the 'basic' information about the processor. The important bit to note is the virtual address information. On page 5 of the BCM2835 peripherals page we see an IO map for the processor. Again, as embedded engineers we must have the IO map to know how to address peripherals on the processor and in some cases how to arrange our linker scripts when there are multiple address spaces.

![Virtual Addresses](http://www.valvers.com/wp-content/uploads/2013/01/arm-c-virtual-addresses.jpg)

The VC CPU Bus addresses relate to the Broadcom Video Core CPU. Although the Video Core CPU is what bootloads from the SD Card, execution is handed over to the ARM core by the time our kernel.img code is called. So we're not interested in the VC CPU Bus addresses.

The ARM Physical addresses is the processors raw IO map when the ARM Memory Management Unit (MMU) is not being used. If the MMU is being used, the virtual address space is what what we'd be interested in.

Before an OS kernel is running, the MMU is also not running as it has not been initialised and the core is running in kernel mode. Addresses on the bus are therefore accessed via their ARM Physical Address. We can see from the IO map that the VC CPU Address 0x7E000000 is mapped to ARM Physical Address 0x20000000. This is important!

Further on in the manual we come across the GPIO peripheral section of the manual (Chapter 6, page 89) .

## Visual Output and Running Code

Finally, let's get on and see some of our code running on the Raspberry-Pi. We'll continue with using the first example of the Cambridge tutorials by lighting the OK LED on the Raspberry-Pi board. This'll be our equivalent of the ubiquitous Hello World example. Normally an embedded Hello World is a blinking LED, so that we know the processor is continuously running. We'll move on to that in a bit.

The GPIO peripheral has a base address in the BCM2835 manual at 0x7E200000. We know from getting to know our processor that this translates to an ARM Physical Address of 0x20200000. This is the first register in the GPIO peripheral register set, the 'GPIO Function Select 0' register.

In order to use an IO pin, we need to configure the GPIO peripheral. From the [Raspberry-Pi schematic diagrams](http://www.raspberrypi.org/wp-content/uploads/2012/10/Raspberry-Pi-R2.0-Schematics-Issue2.2_027.pdf) the OK LED is wired to the GPIO16 line (Sheet 2, B5) . The LED is wired active LOW - this is fairly standard practice. It means to turn the LED on we need to output a 0 (the pin is connected to 0V by the processor) and to turn it off we output a 1 (the pin is connected to VDD by the processor).

Back to the processor manual and we see that the first thing we need to do is set the GPIO pin to an output. This is done by setting the function of GPIO16 to an output. Bits 18 to 20 in the 'GPIO Function Select 1' register control the GPIO16 pin.

In C, we will generate a pointer to the register and use the pointer to write a value into the register. We will mark the register as volatile so that the compiler explicitly does what I tell it to. If we do not mark the register as volatile, the compiler is free to see that we do not access this register again and so to all intents and purposes the data we write will not be used by the program and the optimiser is free to throw away the write because it has no effect.

The effect however is definitely required, but is only externally visible (the mode of the GPIO pin changes). We inform the compiler through the volatile keyword to not take anything for granted on this variable and to simply do as I say with it:
    
    #define GPIO_BASE 0x20200000
    volatile unsigned int* gpio_fs1 = (unsigned int*)(GPIO_BASE+4);

In order to set GPIO16 as an output then we need to write a value of 1 in bits 18-20 of this register. This doesn't mean that we set each bit to one, only that bit 18 must be a 1 and bits 19-20 must be 0. Here we can rely on the fact that this register is set to 0 after a reset and so all we need to do is set bit 18:

    *gpio_fs1 |= (1&lt; &lt;18);

This code looks a bit messy, but we will tidy up and optimise later on. For now we just want to get to the point where we can light an LED and understand why it is lit!

The ARM GPIO peripherals have an interesting way of doing IO. It's actually a bit different to most other processor IO implementations. There is a SET register and a CLEAR register. Writing 1 to any bits in the SET register will SET the corresponding GPIO pins to 1 (logic high), and writing 1 to any bits in the CLEAR register will CLEAR the corresponding GPIO pins to 0 (logic low). There are reasons for this implementation over a register where each bit is a pin and the bit value directly relates to the pins output level, but it's beyond the scope of this tutorial.

So in order to light the LED we need to output a 0. We need to write a 1 to bit 16 in the CLEAR register:

    *gpio_clear |= (1&lt;&lt;16);

Putting what we've learnt into the minimal example above gives us a program that compiles and links into an executable which should provide us with a Raspberry-Pi that lights the OK LED when it is powered. Here's the complete code we'll compile:

    /**
        @file armc-2.c
        @brief ARM C Tutorial #2 for Raspberry-Pi

        Using C without an operating system on the Raspberry-Pi BCM2835 board.

        This example lights the OK LED and then loops infinitely
    */

    /** The base address of the GPIO peripheral (ARM Physical Address) */
    #define GPIO_BASE       0x20200000UL

    #define GPIO_GPFSEL0    0
    #define GPIO_GPFSEL1    1
    #define GPIO_GPFSEL2    2
    #define GPIO_GPFSEL3    3
    #define GPIO_GPFSEL4    4
    #define GPIO_GPFSEL5    5

    #define GPIO_GPSET0     7
    #define GPIO_GPSET1     8

    #define GPIO_GPCLR0     10
    #define GPIO_GPCLR1     11

    #define GPIO_GPLEV0     13
    #define GPIO_GPLEV1     14

    #define GPIO_GPEDS0     16
    #define GPIO_GPEDS1     17

    #define GPIO_GPREN0     19
    #define GPIO_GPREN1     20

    #define GPIO_GPFEN0     22
    #define GPIO_GPFEN1     23

    #define GPIO_GPHEN0     25
    #define GPIO_GPHEN1     26

    #define GPIO_GPLEN0     28
    #define GPIO_GPLEN1     29

    #define GPIO_GPAREN0    31
    #define GPIO_GPAREN1    32

    #define GPIO_GPAFEN0    34
    #define GPIO_GPAFEN1    35

    #define GPIO_GPPUD      37
    #define GPIO_GPPUDCLK0  38
    #define GPIO_GPPUDCLK1  39

    /** GPIO Register set */
    volatile unsigned int* gpio;

    /** Main function - we'll never return from here */
    int main(void)
    {
        /* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
        gpio = (unsigned int*)GPIO_BASE;

        /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
           peripheral register to enable GPIO16 as an output */
        gpio[GPIO_GPFSEL1] |= (1 << 18);

        /* Write 1 to the 16th bit in the Clear0 GPIO peripheral register to set
           the output level of GPIO16 as 0 (logic low) */
        gpio[GPIO_GPCLR0] = (1 << 16);

        /* Never exit as there is no OS to exit to! */
        while(1)
        {

        }
    }

Download the [example here](http://www.valvers.com/files/rpi/tut/armc-2.c).

We now compile with the slightly modified compilation line:

    arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles armc-2.c -o kernel.elf

The linker gives us a warning, which we'll sort out later, but importantly the linker has resolved the problem for us. This is the warning we'll see and ignore:

    c:/gcc-arm-none-eabi-4_7-2013q3-20130916-win32/bin/../lib/gcc/arm-none-eabi/4.7.4/../../../../arm-none-eabi/bin/ld.exe: warning: cannot find entry symbol _start; defaulting to 00008000

As we can see from the compilation, the standard output is ELF format which is essentially an executable wrapped with information that an OS may need to know. We need a binary ARM executable that only includes machine code. We can extract this using the objcopy utility:

    arm-none-eabi-objcopy kernel.elf -O binary kernel.img

### A quick note about  the ELF format

[ELF](http://en.wikipedia.org/wiki/Executable_and_Linkable_Format) is a file format used by some OS, including Linux which wraps the machine code with meta-data.  The meta-data can be useful. In Linux and in fact most OS these days, running an executable doesn't mean the file gets loaded into memory and then the processor starts running from the address at which the file was loaded. There is usually an executable loader which uses formats like ELF to know more about the executable, for example the Function call interface might be different between different executables, this means that code can use different calling conventions which use different registers for different meanings when calling functions within a program. This can determine whether the executable loader will even allow the program to be loaded into memory or not. The ELF format meta-data can also include a list of all of the shared objects (SO, or DLL under Windows) that this executable also needs to have loaded. If any of the required libraries are not available, again the executable loader will not allow the file to be loaded and run.

This is all intended to (and does) increase system stability and compatibility.

We however, do not have an OS and the bootloader does not have any loader other than a disk read, directly copying the kernel.img file into memory at 0x8000 which is then where the ARM processor starts execution of machine code. Therefore we need to strip off the ELF meta-data and simply leave just the compiled machine code in the kernel.img file ready for execution.

### Back to our example

This gives us the kernel.img binary file which should only contain ARM machine code. It should be tens of bytes long. You'll notice that kernel.elf on the otherhand is ~34Kb. Rename the kernel.img on your SD Card to something like old.kernel.img and save your new kernel.img to the SD Card. Booting from this SD Card should now leave the OK LED on permanently. The normal startup is for the OK LED to be on, then extinguish. If it remains extinguished something went wrong with building or linking your program. Otherwise if the LED remains lit, your program has executed successfully.

A blinking LED is probably more appropriate to make sure that our code is definitely running. Let's quickly change the code to crudely blink an LED and then we'll look at sorting out the C library issues we had earlier as the C library is far too useful to not have access to it.

Compile the new code:

    /**
        @file armc-3.c
        @brief ARM C Tutorial #3 for Raspberry-Pi

        Using C without an operating system on the Raspberry-Pi BCM2835 board.

        This example blinks the OK LED infinitely
    */

    /** The base address of the GPIO peripheral (ARM Physical Address) */
    #define GPIO_BASE       0x20200000UL

    #define GPIO_GPFSEL0    0
    #define GPIO_GPFSEL1    1
    #define GPIO_GPFSEL2    2
    #define GPIO_GPFSEL3    3
    #define GPIO_GPFSEL4    4
    #define GPIO_GPFSEL5    5

    #define GPIO_GPSET0     7
    #define GPIO_GPSET1     8

    #define GPIO_GPCLR0     10
    #define GPIO_GPCLR1     11

    #define GPIO_GPLEV0     13
    #define GPIO_GPLEV1     14

    #define GPIO_GPEDS0     16
    #define GPIO_GPEDS1     17

    #define GPIO_GPREN0     19
    #define GPIO_GPREN1     20

    #define GPIO_GPFEN0     22
    #define GPIO_GPFEN1     23

    #define GPIO_GPHEN0     25
    #define GPIO_GPHEN1     26

    #define GPIO_GPLEN0     28
    #define GPIO_GPLEN1     29

    #define GPIO_GPAREN0    31
    #define GPIO_GPAREN1    32

    #define GPIO_GPAFEN0    34
    #define GPIO_GPAFEN1    35

    #define GPIO_GPPUD      37
    #define GPIO_GPPUDCLK0  38
    #define GPIO_GPPUDCLK1  39

    /** GPIO Register set */
    volatile unsigned int* gpio;

    /** Simple loop variable */
    volatile unsigned int tim;

    /** Main function - we'll never return from here */
    int main(void)
    {
        /* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
        gpio = (unsigned int*)GPIO_BASE;

        /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
           peripheral register to enable GPIO16 as an output */
        gpio[GPIO_GPFSEL1] |= (1 << 18);

        /* Never exit as there is no OS to exit to! */
        while(1)
        {
            for(tim = 0; tim &lt; 500000; tim++)
                ;

            /* Set the GPIO16 output low ( Turn OK LED on )*/
            gpio[GPIO_GPCLR0] = (1 << 16);

            for(tim = 0; tim &lt; 500000; tim++)
                ;

            /* Set the GPIO16 output high ( Turn OK LED off )*/
            gpio[GPIO_GPSET0] = (1 << 16);
        }
    }

Download the [example here](http://www.valvers.com/files/rpi/tut/armc-3.c). ...and see the OK LED Blink! :D

Although the code may appear to be written a little odd, please stick with it! There are reasons why it's written how it is. Now you can experiment a bit from a basic starting point, but beware - automatic variables won't work, and nor will initialised variables because we have no C Run Time support yet.

That will be where we start with [Step 2 of Bare metal programming the Raspberry-Pi](http://www.valvers.com/embedded-linux/raspberry-pi/step02-bare-metal-programming-in-c-pt2)!

EDIT: 23/01/14 - Added some more information about the boot process because of a few questions I had emailed to me. Hopefully now the bootloading process is a bit clearer. Also added "A quick note about the ELF format" for clarification of why we use objcopy -O binary

