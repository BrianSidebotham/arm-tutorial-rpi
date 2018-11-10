# Introducing CMake

If you really don't want to know anything about the [CMake](http://www.cmake.org/) build system, just skip this section of the tutorial. There's no ARM bare metal in it - just compiling the same armc-09 tutorial with a build system! However, to continue with the tutorials, you'll need CMake installed to configure + build the tutorials.

The one move we do make in this part of the turotial is move from delay loops to using the system timer peripheral. So the code does change a bit if you want to follow along.

CMake is an excellent build system, I much prefer it to autotools and certainly prefer it in comparison to hand-crafting Makefile's! [CMake's documentation](http://www.cmake.org/cmake/help/v2.8.10/cmake.html) is great, so feel free to peruse it whilst reading this tutorial. The tutorial isn't going to cover CMake, other than showing how it can help and some rudimentary stuff which shows how CMake can be setup to do exactly what we were doing in our simple command-line build.

CMake generally detects the host information, that is the computer you're compiling on and will assume that the target is the same as the host. However, it's very easy to tell CMake not to use that assumption and instead use a system that we're describing or hinting at. We can hint for example that the target is MinGW when the host is Linux and CMake will setup a lot of settings, knowing that they are useful for targetting Windows. On embedded systems we're describing the system ourself. It's very easy to describe to CMake because it doesn't need to know much. It needs to know what the toolchain filename's are and where they're located. Let's look at the toolchain-arm-none-eabi-rpi.cmake that I've added to the arm010 folder:

## toolchain-arm-none-eabi-rpi.cmake

```cmake
#   Part of the Raspberry-Pi Bare Metal Tutorials
#   https://www.valvers.com/rpi/bare-metal/
#   Copyright (c) 2013, Brian Sidebotham

#   This software is licensed under the MIT License.
#   Please see the LICENSE file included with this software.

# A CMake toolchain file so we can cross-compile for the Rapsberry-Pi bare-metal

include(CMakeForceCompiler)

# usage
# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-none-eabi.cmake ../

# The Generic system name is used for embedded targets (targets without OS) in
# CMake
set( CMAKE_SYSTEM_NAME          Generic )
set( CMAKE_SYSTEM_PROCESSOR     BCM2835 )

# Set a toolchain path. You only need to set this if the toolchain isn't in
# your system path. Don't forget a trailing path separator!
set( TC_PATH "" )

# The toolchain prefix for all toolchain executables
set( CROSS_COMPILE arm-none-eabi- )

# specify the cross compiler. We force the compiler so that CMake doesn't
# attempt to build a simple test program as this will fail without us using
# the -nostartfiles option on the command line
CMAKE_FORCE_C_COMPILER( "${TC_PATH}${CROSS_COMPILE}gcc" GNU )
SET( CMAKE_ASM_COMPILER "${TC_PATH}${CROSS_COMPILE}gcc" )

# We must set the OBJCOPY setting into cache so that it's available to the
# whole project. Otherwise, this does not get set into the CACHE and therefore
# the build doesn't know what the OBJCOPY filepath is
set( CMAKE_OBJCOPY      ${TC_PATH}${CROSS_COMPILE}objcopy
    CACHE FILEPATH "The toolchain objcopy command " FORCE )

set( CMAKE_OBJDUMP      ${TC_PATH}${CROSS_COMPILE}objdump
CACHE FILEPATH "The toolchain objdump command " FORCE )

# Set the common build flags

# Set the CMAKE C flags (which should also be used by the assembler!
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-T,${CMAKE_SOURCE_DIR}/rpi.x")

if( "${BOARD}" STREQUAL "rpi0" )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRPI0" )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=vfp" )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv6zk" )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=arm1176jzf-s" )
endif()


set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
```

If you've not used CMake before, some things may look a bit strange - but it's only syntatical. I think you can read CMake files relatively easily even when you've never seen it before.

The toolchain file describes our toolchain. The CMAKE_SYSTEM_NAME is essential, and Generic is used by CMake to essentially set the system type to something CMake doesn't yet know about.

The `CMAKE_SYSTEM_PROCESSOR` is just a string describing the processor type, CMake doesn't use this setting on a system it doesn't know.

The `TC_PATH` (toolchain path) is blank because the toolchain is expected to be in the path. If it's not, then set TC_PATH to something sensible. If you do have to set this - make sure you add a trailing path separator!

The `CROSS_COMPILE` settings is simply the toolchain executable prefix.

The `CMAKE_FORCE_C_COMPILER` setting is the executable name of the c compiler and it's type (GNU), it's mainly built up of variables that we set earlier. We use the FORCE c compiler variable to tell CMake that it shouldn't try to use the compiler before we tell it to. Normally CMake trys to build a blank test program with the compiler to make sure it's working correctly. However, we need the `-nostartfiles` flag in order to build something that succeeds and so CMake's test would fail and it would not it would think the compiler was broken.

The `CMAKE_OBJCOPY` setting looks a bit fancier doesn't it!? CMake knows that `CMAKE_C_COMPILER` needs to be accessible by the entire project, so it adds it automatically to the CACHE. The CACHE is available to all of the CMake files in the build system. If we set a variable in this toolchain file it only has local scope (meaning only this file can see the value we set to this variable). Therefore we force CMake to add this to the cache. The cache requires a "type" for the variable so CMake knows how to use it. In fact most things in CMake are strings. See the documentation for further information on these settings (Look at `commands->set->CACHE` in the documentation).

The `CMAKE_C_FLAGS` settings creates a list of flags that are passed to the compiler when compiling a c source file. Here, we just use the processor-specific flags and again as with OBJCOPY we want to put the `CMAKE_C_FLAGS` setting from here in the CACHE so that the entire project has this variable. We can add more generic flags like optimisation level to the c flags later on because they're not specific to a particular processor. So, notice that we also have `toolchain-arm-none-eabi-rpibplus` and `toolchain-arm-none-eabi-rpi2` for the other board types

As with autotools, CMake uses a configuration step which works out what should be built and how. The result of the configuration step, if successful, is a Makefile which we can use with make (On Windows you must use mingw32-make) to build our project.

CMake uses a file in each directory that needs building called CMakeLists.txt. It's a simple text file as it's extension suggests. Here's the CMakeLists.txt file for arm010:

### CMakeLists.txt

```cmake
#   Part of the Raspberry-Pi Bare Metal Tutorials
#   https://www.valvers.com/rpi/bare-metal/
#   Copyright (c) 2013, Brian Sidebotham

#   This software is licensed under the MIT License.
#   Please see the LICENSE file included with this software.

# CMake build environment for the Valvers Raspberry-Pi bare metal tutorials

# CMake 2.8.10 Documentation: http://www.cmake.org/cmake/help/v2.8.10/cmake.html

cmake_minimum_required( VERSION 2.8 )

# Mark the language as C so that CMake doesn't try to test the C++
# cross-compiler's ability to compile a simple program because that will fail
project( armc-010-tutorial C ASM )

# We had to adjust the CMAKE_C_FLAGS variable in the toolchain file to make sure
# the compiler would work with CMake's simple program compilation test. So unset
# it explicitly before re-setting it correctly for our system
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles" )

add_executable( armc-010-${BOARD}
    armc-010.c
    armc-010-cstartup.c
    armc-010-cstubs.c
    armc-010-start.S )

add_custom_command(
    TARGET armc-010-${BOARD} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ./armc-010-${BOARD}${CMAKE_EXECUTABLE_SUFFIX} -O binary ./kernel-${BOARD}.img
    COMMAND ${CMAKE_OBJDUMP} -l -S -D ./armc-010-${BOARD}${CMAKE_EXECUTABLE_SUFFIX} > ./kernel-${BOARD}.img.asm
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Convert the ELF output file to a binary image" )
```

We specify a minimum version required, I just tend to start at 2.8 because most people have it, but our simple examples in this tutorial can most probably be used with earlier versions than that.

The `project()` call tells CMake what the project is called and what languages are required. The languages can be left out, and frequently are - but there's a few reasons why we need to tell CMake explicitly what languages we're using here.

1. If we don't do this, CMake tests the C++ compiler to make sure it can compile a simple program to validate the C++ compiler. However, as our system is bare metal and requires specific linker steps this fails and CMake falls over at the first hurdle. CMake assumes that C and C++ are the project's languages if you do not tell it any languages explicitly.

2. Assembler files (.S) in the source list are ignored completely by CMake unless we "enable" the assembler by telling CMake in the project definition that we're using assembly files. When we enable assembler, CMake detects gcc as the assembler rather than as - this is good for us because we then only need one set of compilation flags.

As we'll see in a bit, to successfully configure, we need to pass some C flags to CMake when configuring. This is so that the C compiler sanity check passes by enabling the C compiler to successfully compile a small C program.

We then set the C flags that CMake will use when compiling C ( and assembler! ) files. These are exactly the same as we were passing to the gcc invocation in our bat or sh file to compile.

We set the linker flags for when CMake links an executable. Again, this is the same as we were passing to the Linker through gcc when compiling and linking in one command. So CMake will pass this setting to the linker which will make sure we're using our linker script.

Then we add an executable target. A target in CMake is basically something to build. An executable means that CMake knows this target is an executable. If we don't apply any special settings to the executable, the target name becomes the output filename for the target, plus any default prefix or suffix (for example Windows executables have the .exe suffix added). After the target name, we simply list the source files (including header files so that CMake can work out the dependencies for the build process).

Finally we add a custom command associated with the executable target we've just defined. We declare it as being required to be run after building the executable target. This uses objcopy from our toolchain file to extract the binary image from the ELF file to a kernel image.

## Building

So now building the example is slightly different. Navigate into the build sub-directory of the tutorial and run the batch file or bash script called configure. You can look in the script file to see how the configure call is done with CMake. The result of this configure command will be a CodeBlocks project file that you can use to edit, and build the tutorial, and there's a Makefile which the CodeBlocks project uses. You can use this Makefile directly to build the tutorial.

From now on, when I use the term ${MAKE} I mean on Linux use the make command, and on Windows use the `mingw32-make` command. If you've not got mingw32-make on Windows then you best go get it now. I suggest getting the a copy from the mingw-builds project - just download one of the latest toolchains and it will come with mingw32-make.

Build the project - it's the same as the arm009 tutorial, but built with the CMake build system instead. So long as it works for you, this is all then CMake I want to go into - this is an ARM bare metal tutorial, not a build-systems tutorial. However, it's important because soon the number of files we'll have will increase and we do not want to be having a hugely complicated command line to build each tutorial ( or at least, I don't! )

## System Timer Blinking LED

Let's now look at using another peripheral in the Raspberry-Pi processor. We've so far used the GPIO controller to turn an LED on and off, we'll keep that going, but the rate at which we've been turning it on and off is unknown to us. We've just used a volatile loop so the processor takes some amount of time to do the work we've requested (loop and increment a variable!) and then it changes the state of the LED. It'd be much better if we could request that the processor pause for a certain amount of time that is easy to determine in software.

The Raspberry-Pi processor (both BCM2835/6) has a peripheral called the System Clock. It's a 64-bit counter that continuously runs at a fixed rate. It's the introduction of this timer in ARM cores that made porting operating systems (which all need to be time-aware for thread context switching, etc.) easy!! Previously porting an operating system to a new processor, even in the same family of parts could require considerable re-writes.

Again, referring to the [BCM2835-ARM-Peripherals.pdf](http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf), on page 172 is the System Timer peripheral. This is a free-running 64-bit counter as we've already seen.

We need to access these registers, and of the whole peripheral all we have to do is keep an eye on the counter value as it increases. Each increment takes 1us, so keeping track of time using the System Timer is very easy. In C, we can write a "driver" for this peripheral very easily for this basic functionality. We don't even have to initialise the peripheral.

Firstly, we'll need the base address of the system timer:

```c
#define RPI_SYSTIMER_BASE       0x20003000
```

Remember from Pt1 of the tutorial that the physical address 0x7E00000 is mapped to the ARM physical address 0x2000000, so 0x7E003000 becomes 0x20003000 to the processor!

Then we can generate a struct which is structured the same as the registers for the peripheral:

```c
typedef struct {
    volatile uint32_t control_status;
    volatile uint32_t counter_lo;
    volatile uint32_t counter_hi;
    volatile uint32_t compare0;
    volatile uint32_t compare1;
    volatile uint32_t compare2;
    volatile uint32_t compare3;
    } rpi_sys_timer_t;
```

In the driver code, we can declare this type of structure at memory address RPI_SYSTIMER_BASE by using a variable:

```c
static rpi_sys_timer_t* rpiSystemTimer = (rpi_sys_timer_t*)RPI_SYSTIMER_BASE;
```

and then finally, we can implement our delay function:

```c
void RPI_WaitMicroSeconds( uint32_t us )
{
    volatile uint32_t ts = rpiSystemTimer->counter_lo;

    while( ( rpiSystemTimer->counter_lo - ts ) < us )
    {
        /* BLANK */
    }
}
```

Here, all we do is get the current value of the counter wait until the counter is the required number of us higher than when the function was called before returning. As per the Cambridge tutorials, this version ignores the upper 32-bits of the counter. It will take over an hour before the timer increases above 32-bits anyway, so I'm not expecting us to test a blinking LED for that long!

### armc-011.c

```c
#include <string.h>
#include <stdlib.h>

#include "rpi-gpio.h"
#include "rpi-systimer.h"

/** GPIO Register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
    int loop;

    /* Write 1 to the GPIO init nibble in the Function Select GPIO peripheral register to enable
       the LED pin an output */
    gpio[LED_GPFSEL] |= ( 1 << LED_GPFBIT );

    /* Never exit as there is no OS to exit to! */
    while(1)
    {

        /* Set the GPIO16 output high ( Turn OK LED off )*/
        LED_OFF();

        /* Wait half a second */
        RPI_WaitMicroSeconds( 500000 );

        /* Set the GPIO16 output low ( Turn OK LED on )*/
        LED_ON();

        /* Wait half a second */
        RPI_WaitMicroSeconds( 500000 );
    }
}
```

Now we have a slightly slower blinking LED where the time is known - 0.5 seconds before toggling the LED. The system timer can be extremely useful. This is definitely the easiest way to get using a timer on the Raspberry-Pi!

Don't forget to build armc-011 tutorial you need CMake installed. Then you can build with:

```bash
cd arm011\scripts
configure.bat
mingw32-make
```

or on Linux

```bash
cd arm011/scripts
./configure.sh
make
```

I think in the next tutorial part we'll introduce interrupts. The code in this tutorial is far from ideal. Function calls that wait half a second before returning are not what we want. Interrupts can help keep our system more responsive so in that 0.5s we can get on and do other stuff and just let the processor interrupt the code when the 0.5s is up.

I've also included a more fancy use of the timer functions to allow some PWM dimming of the LED so you can check out armc-012 yourself. It's at least a little bit more interesting compared to a normal flashing LED!

When you're ready, [head over to Pt4...](http://www.valvers.com/embedded-linux/raspberry-pi/step04-bare-metal-programming-in-c-pt4)
