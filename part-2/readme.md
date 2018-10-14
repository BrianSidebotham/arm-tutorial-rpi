# Part 2 - The C Runtime

In this part of the tutorial we'll look deeper into the linker and the C-Library so that we end
up with a working C-Library link. Exciting stuff, huh?! Let's look further into what the compiler
and linker are doing in order to create our bare-metal executable.

The C-Runtime (different to the C-Library!) is currently missing from our code. In a lot of
embedded systems the C-Runtime is essential, or else things break instantly. The most notable
thing that's instantly visible in most embedded systems is that static variables are not
initialised.

This is why in our previous example, we were working without pre-initialised variables. Instead,
we initialise the variable in the code at the start of main from a pre-processor define.

> **Github** The code for the tutorials is now on [Github](https://github.com/BrianSidebotham/arm-tutorial-rpi).
> You can either browse the code, checkout the code, fork, branch, or download
> [as a zip](https://github.com/BrianSidebotham/arm-tutorial-rpi/archive/master.zip) from GibHub.

## Understanding the C-Runtime Startup

Let's modify and use a pre-initialised variable instead:

part-2/armc-04:

```c
#include "rpi-gpio.h"

/** GPIO Register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

/** Simple loop variable */
volatile unsigned int tim;

/** Main function - we'll never return from here */
int main(void)
{
    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

    /* Never exit as there is no OS to exit to! */
    while(1)
    {
        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
           for plus models )*/
        gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);

        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
           for plus models )*/
        gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    }
}
```

Compile it (using the build.sh script):

```bash
part-2/armc-04 $ ./build.sh rpi0
arm-none-eabi-gcc -g -nostartfiles -mfloat-abi=hard -O0 -DRPI0 -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s /.../part-2/armc-04/*.c -o /.../part-2/armc-04/kernel.armc-04.rpi0.elf
/.../gcc-arm-none-eabi-7-2017-q4-major/bin/../lib/gcc/arm-none-eabi/7.2.1/../../../../arm-none-eabi/bin/ld: warning: cannot find entry symbol _start; defaulting to 0000000000008000
arm-none-eabi-objcopy /.../part-2/armc-04/kernel.armc-04.rpi0.elf -O binary /.../part-2/armc-04/kernel.armc-04.rpi0.img
```

 and we notice a few things:

- The size of the binary image is now 33k, but the previous version of this code was only a
hundred bytes or so!
- The code, when written to the SDCARD still works - this isn't really expected without a working
C-Runtime in place to initialise the variable gpio before calling main!?

```bash
part-2/armc-04 $ ls -lah
total 40K
drwxr-xr-x 2 brian brian 4.0K Oct 11 21:07 .
drwxr-xr-x 8 brian brian 4.0K Jan  4  2018 ..
-rw-r--r-- 1 brian brian 1.2K Sep 21 00:19 armc-04.c
-rwxr-xr-x 1 brian brian 2.7K Oct 11 21:07 build.sh
-rwxr-xr-x 1 brian brian  108 Sep 21 00:19 disassemble.sh
-rwxr-xr-x 1 brian brian  35K Oct 11 21:07 kernel.armc-04.rpi0.elf
-rwxr-xr-x 1 brian brian  65K Oct 11 21:07 kernel.armc-04.rpi0.img
-rw-r--r-- 1 brian brian 1.7K Sep 21 00:19 rpi-gpio.h
```

In fact, this embedded system is different to a lot because we're loading an entire binary image
into RAM and then executing from RAM. The majority of systems have a non-volatile memory section
(Flash/ROM) where the executable code resides, and a volatile memory section (RAM) where the
variable data resides. Variables exist in RAM, everything has a position in RAM.

When we compile for a target that executes from an image in Flash and uses RAM for variables, we
need a copy of the initial values for the variables from Flash so that every time the system is
started the variables can be initialised to their initial value, and we need the code in Flash to
copy these values into the variables *before* `main()` is called.

This is one of the jobs of the C-Runtime code (CRT). This is a code object that is normally
linked in automagically by your tool-chain. This is usually not the only object to get linked
to your code behind your back - usually the Interrupt Vector Table gets linked in too, and a
Linker Script tells the linker how to organise these additional pieces of code in your memory
layout.

Normally of course, this happens without you knowing. In general, you'll select your processor
or embedded system on the command line and the appropriate linker script and C-Runtime is
chosen for you and linked in.

I urge you to go and look at your `arm-none-gcc-eabi` install now to see some of these files.
Look under the `arm-none-eabi` sub-directory and then under the lib sub-directory. The C-Runtime
code is a binary object file and is called `crt0.o`, the C Library for info is an archive of
object files called libc.a (there may be several versions with different names), and then you'll
have some `.ld` files. Under the ldscripts subdirectory you'll find the standard linker scripts.
It's just worth a look to know they're there. GCC uses a thing called specs files too, which
allow specifying system settings so that you can create a machine specification that allows you
to target a machine easily. You can select a custom specs file with a command line option for
GCC, otherwise gcc uses it's built-in specs.

specs files are considered an advanced subject in the world of the GNU tool-chain, but they
provide an excellent way of supplying machine-specific compilation settings. For the embedded
engineer they're worth knowing about! :D

So, now we've got two questions, why does our code work - because the initialisation isn't present
in the C-Runtime? and, why has our code size jumped from a 100 bytes or so to 64k?

The code works without any initialisation because the variables exist in the same memory space
as the code. The bootloading process results in the raspberry-pi kernel being loaded into RAM
in order to be executed, the GPU bootloader runs before the ARM processor we're targeting runs,
and loads the kernel.img file from disk. Because of this, the variables position within the
binary image becomes their variable memory location. The image is loaded by the boot-loader at
address `0x8000` and then executed. So the bootloader has essentially done a taskt that the
C-Runtime would normally do, copy the initial values of initialised variables from non-volatile
memory to volatile memory. Cool.

Look at the code produced closer with a disassembler. You've already got a disassembler!
It comes with the toolchain; Welcome to the world of objdump (or in our case `arm-non-eabi-objdump`).
We disassemble the elf file because then objdump knows what processor the binary was built for.
It also then has knowledge of the different code sections too. There's a `disassemble.sh` script
so go ahead and disassemble the code to see what the compiler generated. You'll get a `kernel*.asm`
file that looks similar to if not the same as the following (RPI0) code:

```
Disassembly of section .text:

00008000 <main>:
    8000:    e59f30b8     ldr    r3, [pc, #184]    ; 80c0 <main+0xc0>
    8004:    e5933000     ldr    r3, [r3]
    8008:    e2833004     add    r3, r3, #4
    800c:    e5932000     ldr    r2, [r3]
    8010:    e59f30a8     ldr    r3, [pc, #168]    ; 80c0 <main+0xc0>
    8014:    e5933000     ldr    r3, [r3]
    8018:    e2833004     add    r3, r3, #4
    801c:    e3822701     orr    r2, r2, #262144    ; 0x40000
    8020:    e5832000     str    r2, [r3]
    8024:    e59f3098     ldr    r3, [pc, #152]    ; 80c4 <main+0xc4>
    8028:    e3a02000     mov    r2, #0
    802c:    e5832000     str    r2, [r3]
    8030:    ea000004     b    8048 <main+0x48>
    8034:    e59f3088     ldr    r3, [pc, #136]    ; 80c4 <main+0xc4>
    8038:    e5933000     ldr    r3, [r3]
    803c:    e2833001     add    r3, r3, #1
    8040:    e59f207c     ldr    r2, [pc, #124]    ; 80c4 <main+0xc4>
    8044:    e5823000     str    r3, [r2]
    8048:    e59f3074     ldr    r3, [pc, #116]    ; 80c4 <main+0xc4>
    804c:    e5933000     ldr    r3, [r3]
    8050:    e59f2070     ldr    r2, [pc, #112]    ; 80c8 <main+0xc8>
    8054:    e1530002     cmp    r3, r2
    8058:    9afffff5     bls    8034 <main+0x34>
    805c:    e59f305c     ldr    r3, [pc, #92]    ; 80c0 <main+0xc0>
    8060:    e5933000     ldr    r3, [r3]
    8064:    e2833028     add    r3, r3, #40    ; 0x28
    8068:    e3a02801     mov    r2, #65536    ; 0x10000
    806c:    e5832000     str    r2, [r3]
    8070:    e59f304c     ldr    r3, [pc, #76]    ; 80c4 <main+0xc4>
    8074:    e3a02000     mov    r2, #0
    8078:    e5832000     str    r2, [r3]
    807c:    ea000004     b    8094 <main+0x94>
    8080:    e59f303c     ldr    r3, [pc, #60]    ; 80c4 <main+0xc4>
    8084:    e5933000     ldr    r3, [r3]
    8088:    e2833001     add    r3, r3, #1
    808c:    e59f2030     ldr    r2, [pc, #48]    ; 80c4 <main+0xc4>
    8090:    e5823000     str    r3, [r2]
    8094:    e59f3028     ldr    r3, [pc, #40]    ; 80c4 <main+0xc4>
    8098:    e5933000     ldr    r3, [r3]
    809c:    e59f2024     ldr    r2, [pc, #36]    ; 80c8 <main+0xc8>
    80a0:    e1530002     cmp    r3, r2
    80a4:    9afffff5     bls    8080 <main+0x80>
    80a8:    e59f3010     ldr    r3, [pc, #16]    ; 80c0 <main+0xc0>
    80ac:    e5933000     ldr    r3, [r3]
    80b0:    e283301c     add    r3, r3, #28
    80b4:    e3a02801     mov    r2, #65536    ; 0x10000
    80b8:    e5832000     str    r2, [r3]
    80bc:    eaffffd8     b    8024 <main+0x24    >
    80c0:    000180cc     andeq    r8, r1, ip, asr #1
    80c4:    000180d0     ldrdeq    r8, [r1], -r0
    80c8:    0007a11f     andeq    sl, r7, pc, lsl r1

Disassembly of section .data:

000180cc <gpio>:
   180cc:    20200000     eorcs    r0, r0, r0

Disassembly of section .bss:

000180d0 <tim>:
   180d0:    00000000     andeq    r0, r0, r0
```

>**NOTE:** Unless we're using the exact same compiler, your mileage may vary here. So assume the
> assembly code above is what's come out of the compiler and follow the text below which goes
> through it in detail.

Let's take it line by line. The toolchain's linker has decided that the entry point for the code
should be at memory address `0x8000`. We see from the disassembled listing that this is where the
machine code starts. Let's look at what it does.

```
00008000 <main>:
    8000:	e59f30b8 	ldr	r3, [pc, #184]	; 80c0 <main+0xc0>
```

This first line loads r3 with the value at the *address* contained at the Program Counter
(PC) + 184. In this assembler, `[]` is kind of an equivalent to dereferencing a pointer in C. So
instead of loading r3 with the value `80c0` it will instead load r3 with the 32-bit value at
memory location `0x80c0`

But wait a minute - if you do the maths of `0x8000 + 184` you get `0x80b8`. How come the
disassembler is suggesting the data comes from `0x80c0`?

Well PC relative addressing works slightly different to what you may expect and really there's a
issue with it, the value of PC is different depending on whether the instruction set is currently
ARM or Thumb. Further information is available on the [ARM website](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473e/Cacdbfji.html).
The important thing to note here is:

> In ARM state, the value of the PC is the address of the current instruction plus 8 bytes.
>
> In Thumb state:
>
> - For B, BL, CBNZ, and CBZ instructions, the value of the PC is the address of the current
> instruction plus 4 bytes.
>
> - For all other instructions that use labels, the value of the PC is the address of the current
> instruction plus 4 bytes, with bit[1] of the result cleared to 0 to make it word-aligned.

We're in ARM instruction mode here. If you're not sure about ARM and Thumb instruction sets you can
do some googling. Thumb (there's more than one Thumb mode!) are smaller width instructions to allow
for more compact code which is very useful in heavily embedded systems.

So actually the maths here sound be: `0x8000 + 0x8 + 0xb8 = 0x80c0` to get us to a memory address.
At `0x80c0` there's the value `0x180cc` (`0x8000 + 0x80cc`):

```
    80c0:	000180cc 	andeq	r8, r1, ip, asr #1
```

You can ignore the disassembled version of this value as it's not machine code, instead it's merely
a data value.

The next line of the code loads r3 with the 32-bit value that's in memory at the address currently
contained in r3. In c, this would look a bit horrid, but so you get the idea of what I've just
explained, imagine something like this:

```c
uint32_t r3 = 0x180cc;
r3 = *(uint32_t*)r3;
```

At the address 0x180cc in our disassembled version there's the 32-bit value `0x20200000` which is
the value we want the variable gpio initialised to (for the RPI1, for the RPI2 this will be
`0x3F200000`. So this is why the code works without any explicit loading or initialisation, but
let's look at exactly what's going on and find out why it works like this.

The value at the end of our executable image can be viewed by dumping the hex and having a look at
the plain machine code that's in the binary file. The disassemble script does this for you with a
tool called `hexdump` and puts the result into a `kernel*.img.hexdump` file. It's a plain text file -
you can go ahead and crack it open in a text editor or `cat` it.

```
0000000 30b8 e59f 3000 e593 3004 e283 2000 e593
0000010 30a8 e59f 3000 e593 3004 e283 2701 e382
0000020 2000 e583 3098 e59f 2000 e3a0 2000 e583
0000030 0004 ea00 3088 e59f 3000 e593 3001 e283
0000040 207c e59f 3000 e582 3074 e59f 3000 e593
0000050 2070 e59f 0002 e153 fff5 9aff 305c e59f
0000060 3000 e593 3028 e283 2801 e3a0 2000 e583
0000070 304c e59f 2000 e3a0 2000 e583 0004 ea00
0000080 303c e59f 3000 e593 3001 e283 2030 e59f
0000090 3000 e582 3028 e59f 3000 e593 2024 e59f
00000a0 0002 e153 fff5 9aff 3010 e59f 3000 e593
00000b0 301c e283 2801 e3a0 2000 e583 ffd8 eaff
00000c0 80cc 0001 80d0 0001 a11f 0007 0000 0000
00000d0 0000 0000 0000 0000 0000 0000 0000 0000
*
00100c0 0000 0000 0000 0000 0000 0000 0000 2020
00100d0
```

The binary is in little-endian format because the processor is little endian. Adjust the
`GPIO_BASE` value, recompile and disassemble again so you can see the value change. Sure the
code won't work properly, but you can prove to yourself you're looking (and making sense of) the
right thing.

The thing to note here is that our binary image ends at `0x100cf`. After the bootloader has placed
this binary image in memory at `0x8000` the last memory location we touch is `0x180cf`.

Let's have a quick look at what happens next:

```
8008:	e2833004 	add	r3, r3, #4
```

Increase the value in r3 from `0x20200000` to `0x20200004` and store it in r3. This is part of
the C line:

```c
gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);
```

LED_GPFSEL for this compilation for the RPI Zero is set by:

```c
#define LED_GPFSEL      GPIO_GPFSEL1
#define GPIO_GPFSEL1    1
```

From the definition of `gpio` as `volatile unsigned int*` each item pointed to is an unsigned int
which is 32-bit wide. That's 4 bytes, so we increase the pointer into gpio by 4 bytes to get the
register address we require in r3.

```
800c:	e5932000 	ldr	r2, [r3]
```

Load the value of `gpio[LED_GPFSEL]` into r2. This is common for read-modify-write operations
such as the `|=` operator we're using in this example. We need to do exactly that, read the
value, modify the value and then write the new value.

```
8010:	e59f30a8 	ldr	r3, [pc, #168]	; 80c0 <main+0xc0>
8014:	e5933000 	ldr	r3, [r3]
8018:	e2833004 	add	r3, r3, #4
```

The compiler being rather inefficient here. It's preparing r3 again to be the memory write
destination which is exactly the same read memory location. Any sort of compiler optimisations
should be able to get rid of the above three lines. They're really not required, but we
understand what's going on still.

```
801c:	e3822701 	orr	r2, r2, #262144	; 0x40000
```
The modify part of `|=`. In this case `(1 << LED_GPFBIT)` has been reduced to a constant.
The original `gpio[LED_GPFSEL]` value is still in r2. We OR that value with the constant to
set the bit and store the new value in r2. The ARM architecture cannot modify a register value
and store it to a memory location in a single instruction BTW in case you're thinking that
would serve us better.

```
8020:	e5832000 	str	r2, [r3]
```

Finally, the write part of the read-modify-write operation is to write the new value back to
the destination `gpio[LED_GPFSEL]`

```
8024:	e59f3098 	ldr	r3, [pc, #152]	; 80c4 <main+0xc4>
...
80c4:	000180d0 	ldrdeq	r8, [r1], -r0
```

Loads the value `0x180d0` into r3.

This value is a problem! It is outside our binary image space. If we were to use this value it
would be a random value that isn't within our control. How come?

This memory location relates to the C variable `tim` in our program. This variable is determined
by the C standard to have automatic storage duration and according to the C standard should
therefore have a initialised value of 0, but we know here that this is not the case, instead
we'll have a random value.

Actually what happens in the code next is the `for` loop starts by setting the `tim` variable to `0`.

```
8028:	e3a02000 	mov	r2, #0
802c:	e5832000 	str	r2, [r3]
```

Let's do a sanity check and make sure we're right by assuming that this is the `tim` variable:

```
000180d4 B __bss_end__
000180d4 B _bss_end__
000180d0 B __bss_start
000180d0 B __bss_start__
000180cc D __data_start
000180d0 D _edata
000180d4 B _end
000180d4 B __end__
000180cc D gpio
00008000 T main
00080000 N _stack
         U _start
000180d0 B tim
```

Yep! We're talking the same as [nm](https://sourceware.org/binutils/docs/binutils/nm.html).
That's good at least!

Now let's do an experiment. There's a reason I decoded the value `0x100cc` which
is the address of the gpio variable as `0x8000 + 0x80cc` earlier:

## part-2/armc05.c

```c

#include "rpi-gpio.h"

/** GPIO Register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

/** Simple loop variable */
volatile unsigned int tim;

/** Main function - we'll never return from here */
int main(void) __attribute__((naked));
int main(void)
{
    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

    /* Never exit as there is no OS to exit to! */
    while(1)
    {
        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
           for plus models )*/
        gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);

        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
           for plus models )*/
        gpio[LED_GPSET] = (1 << LED_GPIO_BIT);

        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
           for plus models )*/
        gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);
    }
}
```

`armc-05.c` varies only slightly from `armc-04.c` in that it adds in some code. When we look at this
with `nm` we see that the `__data_start` section shifts by the same amount as the code we've added:

```
part-2/armc-05 $ ./build.sh rpi0
...

part-2/armc-05 $ ./disassemble.sh
...

part-2/armc-05 $ cat kernel.armc-05.rpi0.elf.nm
 00018120 B __bss_end__
 00018120 B _bss_end__
 0001811c B __bss_start
 0001811c B __bss_start__
 00018118 D __data_start
 0001811c D _edata
 00018120 B _end
 00018120 B __end__
 00018118 D gpio
 00008000 T main
 00080000 N _stack
          U _start
 0001811c B tim
```

If we go back to the original disassembled output above, we see that address
`0x80cc (0x180cc - (0x8000 * 2))` is the next available memory address after the constants data
which is itself immediately after the code section.

I hope you're following this, we're really seeing a "bug" in the linker script. There's no need
for us to have this offset. The offset load is correct because when we load this image in RAM
the data is indeed going to be `+ 0x8000` because that's where the boot-loader is going to place
the image, but something is spacing the data section away from the constants data.

There's a lot more output when building this code. That's because we've added an option
(`-Wl,-verbose`) to get verbose output from the linker to see what's going on.

As you see from the start of the ld verbose output:

```
GNU ld (GNU Tools for Arm Embedded Processors 7-2017-q4-major) 2.29.51.20171128
  Supported emulations:
   armelf
using internal linker script:
```

LD is using an internal linker script. That means the linker script it's using is compiled into
the ld executable.

## part-2/armc-06.c

The code is identical to `armc-04.c`. (So the original code without the additional loop in
`armc-05.c`)

However, we add `-Wl,-T,rpi.x` to the compilation command to instruct LD to use a different
linker script to the one it was using. We've retained `-Wl,verbose` so we can see what happens.

When passing options to the linker using GCC to compile and link, check
the [gcc documentation](http://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) which details the
options you can pass.

Now we have control of the linker script. We can try to find out what's "wrong". I quote wrong,
because technically this works, but we've got an annoying 0x8000 offset. It'll be a pain to
debug one day, I know it. Let's find out what we need to do to fix it instead. You can also see
how complicated a linker script can get when it needs to deal with C++ sections!

I hope you're staying with me! I know this may seem far removed from C development on the
Raspberry-Pi bare-metal, but in fact knowing how your tools work to construct the code is
essential later on down the line - heck you wouldn't be getting it up and running on your own
from scratch unless you knew at least some of this stuff!

Whilst I'm scan-reading the linker script I'm looking for things that stick out. Most of these
sections are not used in our basic example so far, so whatever is messing with us must have a
size set, and must be present only when we have something in the initialised variables section
which we know from our investigations with nm is the `__data_start` section (it aligns with
the gpio symbol which we initialised).

The "problem" must be between the `__data_start` section and end of the text section (the
symbol main indicated the start of the text section). After a little bit of hunting, I see a
comment and an alignment pertaining to the data section on line 107:

```
/* Adjust the address for the data segment.  We want to adjust up to
     the same address within the page on the next page up.  */
  . = ALIGN(CONSTANT (MAXPAGESIZE)) + (. & (CONSTANT (MAXPAGESIZE) - 1));
```

Interesting, we apparently "want" to adjust up to the next page. If I comment this out with a
standard C-style comment as can be seen in the rest of the file, we get back down to a size of a
few hundred bytes with initialised data. Check again with the output of:

```
part-2/armc-06 $ ll

-rwxr-xr-x 1 brian brian   284 Oct 14 22:17 kernel.armc-06.rpi0.img*
```

When we disassemble the kernel code, we end up with something similar to the following:

```
part-2/armc-06 $ ./disassemble.sh
```

```
Disassembly of section .text:

00008000 <main>:
    8000:	e59f30b8 	ldr	r3, [pc, #184]	; 80c0 <main+0xc0>
    8004:	e5933000 	ldr	r3, [r3]
    8008:	e2833010 	add	r3, r3, #16
    800c:	e5932000 	ldr	r2, [r3]
    8010:	e59f30a8 	ldr	r3, [pc, #168]	; 80c0 <main+0xc0>
    8014:	e5933000 	ldr	r3, [r3]
    8018:	e2833010 	add	r3, r3, #16
    801c:	e3822602 	orr	r2, r2, #2097152	; 0x200000
    8020:	e5832000 	str	r2, [r3]
    8024:	e59f3098 	ldr	r3, [pc, #152]	; 80c4 <main+0xc4>
    8028:	e3a02000 	mov	r2, #0
    802c:	e5832000 	str	r2, [r3]
    8030:	ea000004 	b	8048 <main+0x48>
    8034:	e59f3088 	ldr	r3, [pc, #136]	; 80c4 <main+0xc4>
    8038:	e5933000 	ldr	r3, [r3]
    803c:	e2833001 	add	r3, r3, #1
    8040:	e59f207c 	ldr	r2, [pc, #124]	; 80c4 <main+0xc4>
    8044:	e5823000 	str	r3, [r2]
    8048:	e59f3074 	ldr	r3, [pc, #116]	; 80c4 <main+0xc4>
    804c:	e5933000 	ldr	r3, [r3]
    8050:	e59f2070 	ldr	r2, [pc, #112]	; 80c8 <main+0xc8>
    8054:	e1530002 	cmp	r3, r2
    8058:	9afffff5 	bls	8034 <main+0x34>
    805c:	e59f305c 	ldr	r3, [pc, #92]	; 80c0 <main+0xc0>
    8060:	e5933000 	ldr	r3, [r3]
    8064:	e283302c 	add	r3, r3, #44	; 0x2c
    8068:	e3a02902 	mov	r2, #32768	; 0x8000
    806c:	e5832000 	str	r2, [r3]
    8070:	e59f304c 	ldr	r3, [pc, #76]	; 80c4 <main+0xc4>
    8074:	e3a02000 	mov	r2, #0
    8078:	e5832000 	str	r2, [r3]
    807c:	ea000004 	b	8094 <main+0x94>
    8080:	e59f303c 	ldr	r3, [pc, #60]	; 80c4 <main+0xc4>
    8084:	e5933000 	ldr	r3, [r3]
    8088:	e2833001 	add	r3, r3, #1
    808c:	e59f2030 	ldr	r2, [pc, #48]	; 80c4 <main+0xc4>
    8090:	e5823000 	str	r3, [r2]
    8094:	e59f3028 	ldr	r3, [pc, #40]	; 80c4 <main+0xc4>
    8098:	e5933000 	ldr	r3, [r3]
    809c:	e59f2024 	ldr	r2, [pc, #36]	; 80c8 <main+0xc8>
    80a0:	e1530002 	cmp	r3, r2
    80a4:	9afffff5 	bls	8080 <main+0x80>
    80a8:	e59f3010 	ldr	r3, [pc, #16]	; 80c0 <main+0xc0>
    80ac:	e5933000 	ldr	r3, [r3]
    80b0:	e2833020 	add	r3, r3, #32
    80b4:	e3a02902 	mov	r2, #32768	; 0x8000
    80b8:	e5832000 	str	r2, [r3]
    80bc:	eaffffd8 	b	8024 <main+0x24>
    80c0:	000080cc 	andeq	r8, r0, ip, asr #1
    80c4:	000080d0 	ldrdeq	r8, [r0], -r0
    80c8:	0007a11f 	andeq	sl, r7, pc, lsl r1

Disassembly of section .data:

000080cc <gpio>:
    80cc:	20200000 	eorcs	r0, r0, r0

Disassembly of section .bss:

000080d0 <tim>:
    80d0:	00000000 	andeq	r0, r0, r0
```

Use the [ARM instruction set quick reference card](http://infocenter.arm.com/help/topic/com.arm.doc.qrc0001l/QRC0001_UAL.pdf)
to decypher if you're not familiar with ARM assembler.

By the way, as a quick note - the comment in the linker script is entirely useless as it only
describes what the code is doing, the worst type of comment! There is no *WHY* in the comment.
Why are we wanting to do this? What's the reason for forcing the alignment? Anyway this alignment
was forcing the `0x8000` additional offset to our rodata (initialised data) section.

Cool, now the initialised data is tagged immediately on the end of the data and the values
look sane too! I'll leave you to decode the assembly.

## part-2/armc-07

The code here is the same as `armc-06.c`. Nothing has changed, we'll just modify our linker
script. At this point, we can get rid of the annoying _start undefined symbol warning. At the
top of the linker script, on line 9 you'll see a line that says `ENTRY(_start)`

In `armc-07` we've changed this to read `ENTRY(main)`

The warning will go away at last! But we'll soon learn why we need that `_start` section
for the C-Runtime anyway!

Have a quick check of the sections with nm again, now there's no Undefined sections!

```
part-2/armc-07 $ ./build.sh rpi0
arm-none-eabi-gcc -g -nostartfiles -mfloat-abi=hard -O0 -DRPI0 -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s part-2/armc-07/*.c -o part-2/armc-07/kernel.armc-07.rpi0.elf
arm-none-eabi-objcopy part-2/armc-07/kernel.armc-07.rpi0.elf -O binary part-2/armc-07/kernel.armc-07.rpi0.img
```

```
part-2/armc-07 $ ./disassemble.sh
```

```
part-2/armc-07 $ cat ./kernel.armc-07.rpi0.elf.nm
000080d4 B __bss_end__
000080d4 B _bss_end__
000080d0 B __bss_start
000080d0 B __bss_start__
000080cc D __data_start
000080d0 D _edata
000080d4 B _end
000080d4 B __end__
000080cc D gpio
00008000 T main
00080000 N _stack
000080d0 B tim
```

## Sections and C Startup

As we've found out, there are a lot of sections, some which you may not know the meaning of. The
`.bss` section is used for data that is implicitly initialised to 0 at startup (This is mandated
by the C Standard). This means that all variables that are statically declared are set to zero
initially. Statically declared essentially means global, so local (automatic) variables in a
function that are not marked as static do not get initialised to zero if you do not explicitly
add an initial value. It's easier with an example, take for example the following C file:

```c
    unsigned int var1;
    unsigned int var2;
    unsigned int var3 = 10;

    void function( void )
    {
        unsigned int funcvar1;
        static unsigned int funcvar2;

        /* ... */

    }
```

In the above example var1, var2 and funcvar2 are in the bss section, the rest are not. For more
information on the bss section, see the [wikipedia page on it](http://en.wikipedia.org/wiki/.bss)

The linker organises the data and sorts it out into the different sections. In the above, var3
for example goes into the data section. The code is compiled into machine code and then put in
the `.text` section. See [here](http://en.wikipedia.org/wiki/Code_segment) for "a little!" more
information about the text section. There can be many text sections and there can be many data
sections too. These sections are wild-carded in the linker script to ensure different sections
can be defined whilst still knowing whether they are code or data sections.

In our current code (armc-07) the bss section is not valid because it is not being initialised.
That's because the C-Startup is missing. This is the importance of the `_start` symbol!

The `_start` section is run before the c main() entry point and one of it's jobs is to initialise
the bss section. The linker provides us with a couple of symbols for the sections so we know
where they start and end. In the startup code all we need to do is loop between the addresses
defined by `bss_start` and `bss_end` and set all locations to zero. It's easy when you know
what you're meant to do!

The `_startup` code should also setup the stack pointer. We'll need a working stack to get
anything useful up and working! The stack is temporary memory space available for functions
to use. The compiler tries to use registers for local (automatic) function variables, but if
the size of data required by the local variables exceeds the amount of registers available,
the compiler uses the stack for the local variables.

So lets go ahead and setup the stack pointer to a sane value and initialise the bss section.

Firstly, we'll set the linker script back to standard so the entry point is the `_start` symbol
again, we'll have to generate this symbol and generally we'll need to do this is assembler. We
need to setup the stack pointer before we enter the C code so that we're safe to write C which
may attempt to use the stack straight away.

Normally the complete startup is done in assembler, including zeroing the bss section, but it
doesn't have to! I prefer to get into C as soon as possible, so let's see how little assembler
we can get away with, and see what trips us up next!

## armc-08-start.S

```
.section ".text.startup"

.global _start

_start:
    // Set the stack pointer, which progresses downwards through memory
    // Set it at 64MB which we know our application will not crash into
    // and we also know will be available to the ARM CPU. No matter what
    // settings we use to split the memory between the GPU and ARM CPU
    ldr     sp, =0x8000

    // Run the c startup function - should not return and will call kernel_main
    b       _cstartup

_inf_loop:
    b       _inf_loop
```

Not bad! There's not exactly a lot there to set up the stack pointer. The stack is placed at the
start of our program and grows downwards through memory. You don't need a large amount of memory
for the stack.

Some important information about the `.section` declaration. With the linker script, I can see
in `rpi.x` that the following text sections are available:

```
.text           :
{
  *(.text.unlikely .text.*_unlikely .text.unlikely.*)
  *(.text.exit .text.exit.*)
  *(.text.startup .text.startup.*)
  *(.text.hot .text.hot.*)
  *(.text .stub .text.* .gnu.linkonce.t.*)
  /* .gnu.warning sections are handled specially by elf32.em.  */
  *(.gnu.warning)
  *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)
}
```

This is also the order in which the text section is linked together by the linker. The standard
functions generally go in the standard `.text` section. Therefore, we can put our `_startup`
function before the main `.text` section by putting it in the `.text.startup` section.
Other than `.text.unlikely` and `text.exit` which we won't use, `_startup` will be the first
thing to go into the text segment which is where execution will start at `0x8000`

Then, we're in C for initialising the bss section by running the `cstartup` function:

## armc-08-cstartup.c

```c
extern int __bss_start__;
extern int __bss_end__;

extern void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags );

void _cstartup( unsigned int r0, unsigned int r1, unsigned int r2 )
{
    /*__bss_start__ and __bss_end__ are defined in the linker script */
    int* bss = &__bss_start__;
    int* bss_end = &__bss_end__;

    /*
        Clear the BSS section

        See http://en.wikipedia.org/wiki/.bss for further information on the
            BSS section

        See https://sourceware.org/newlib/libc.html#Stubs for further
            information on the c-library stubs
    */
    while( bss < bss_end )
        *bss++ = 0;

    /* We should never return from main ... */
    kernel_main( r0, r1, r2 );

    /* ... but if we do, safely trap here */
    while(1)
    {
        /* EMPTY! */
    }
}
```

Again, not that bad and pretty reasonable. **NOTE:** We've now changed from main to `kernel_main`,
and there's a reason for this - the bootloader is actually expecting a slightly different entry
definition compared to the standard C main function. So as we're setting up our own C-Runtime
anyway, we can define the correct entry format. The correct bootloader entry point defines a
couple of values which we can check to know what system we're booting.

The actual C code hasn't really changed apart from the `kernel_main` difference:

## armc-08.c

```c
#include "rpi-gpio.h"

/** GPIO Register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

/** Simple loop variable */
volatile unsigned int tim;

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

    /* Never exit as there is no OS to exit to! */
    while(1)
    {
        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
           for plus models )*/
        gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);

        for(tim = 0; tim < 500000; tim++)
            ;

        /* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
           for plus models )*/
        gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    }
}
```

We again confirm with nm that everything is ordered at least something like sensible:

```
part-2/armc-08 $ ./build.sh rpi0
arm-none-eabi-gcc -g -nostartfiles -mfloat-abi=hard -O0 -DRPI0 -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s part-2/armc-08/*.S part-2/armc-08/*.c -o part-2/armc-08/kernel.armc-08.rpi0.elf
arm-none-eabi-objcopy part-2/armc-08/kernel.armc-08.rpi0.elf -O binary part-2/armc-08/kernel.armc-08.rpi0.img
```

```
brian@brian-PH67-UD3-B3 ~/valvers-new/arm-tutorial-rpi/part-2/armc-08 $ ./disassemble.sh
```

```
part-2/armc-08 $ cat ./kernel.armc-08.rpi0.elf.nm

00008164 B __bss_end__
00008164 B _bss_end__
00008160 B __bss_start
00008160 B __bss_start__
0000800c T _cstartup
0000815c D __data_start
00008160 D _edata
00008164 B _end
00008164 B __end__
0000815c D gpio
00008008 t _inf_loop
00008078 T kernel_main
00080000 N _stack
00008000 T _start
00008160 B tim
```

Yup, looks good - `_start` is at `0x8000` where execution will start. Stick it on the card and
make sure the OK LED is blinking still. I appreciate that we're doing a lot of work where you
can't see more interesting results - but it'll get better soon, I promise! For now we're getting
a great foundation for developing in C for the bare metal Raspberry-Pi. Knowing what your tools
are doing is pretty essential!

```
part-2/armc-08 $ ./make_card.sh rpi0
part-2/armc-08 $ cat ./card.armc-08.rpi0.img > /dev/sdg && sync && eject /dev/sdg
```

## The C-Library stubs

Now that we've got the C-Runtime pretty much setup, we can implement the C-Library again with
our own C-Runtime startup and C-Library stubs. This is where we want to get to - easy compiling
with the C-Library being linked in so that we can make use of the C-Library functions on the
Raspberry-Pi without an operating system.

We are actually linking against the C-Library, but we're not using anything from within it.
Therefore, the linker disregards the whole of the C-Library because there are no references to
it within the code. Let's jump straight in and malloc some memory as I know that requires a
stub. Compile `armc-09` without the `*-cstubs.c` and you'll see the error:

```
.../arm-none-eabi/lib/fpu\libg.a(lib_a-sbrkr.o): In function `_sbrk_r':
sbrkr.c:(.text._sbrk_r+0x18): undefined reference to `_sbrk'
collect2.exe: error: ld returned 1 exit status
```

So the malloc call is calling something in the c library called `_sbrk_r` which is a re-entrant
safe (or multi-thread/interrupt safe) C library function all called `_sbrk`. This is the C-stub
function that we must implement as it is operating-system dependant.

It's worth looking at how other people have implemented these function calls. Look at them
to see what they're doing. Generally you can look at other kernel code, or embedded system
code:

[http://www.opensource.apple.com/source/Libc/Libc-763.12/emulated/brk.c](http://www.opensource.apple.com/source/Libc/Libc-763.12/emulated/brk.c)

[http://linux.die.net/man/2/sbrk](http://linux.die.net/man/2/sbrk)

However, the best place is if examples are in the C-Library you're using! Newlib is well
documented and [includes example minimal systemc calls/stubs](https://sourceware.org/newlib/libc.html#Syscalls).

We implement this in the next tutorial `armc-09`

## armc-09-cstubs.c

```c
#include <sys/stat.h>

/* A helper function written in assembler to aid us in allocating memory */
extern caddr_t _get_stack_pointer(void);


/* Increase program data space. As malloc and related functions depend on this,
   it is useful to have a working implementation. The following suffices for a
   standalone system; it exploits the symbol _end automatically defined by the
   GNU linker. */
caddr_t _sbrk( int incr )
{
    extern char _end;
    static char* heap_end = 0;
    char* prev_heap_end;

    if( heap_end == 0 )
        heap_end = &_end;

     prev_heap_end = heap_end;

     heap_end += incr;
     return (caddr_t)prev_heap_end;
}
```

## armc-09-start.S

```
.section ".text.startup"

.global _start
.global _get_stack_pointer

_start:
    // Set it at 64MB which we know our application will not crash into
    // and we also know will be available to the ARM CPU. No matter what
    // settings we use to split the memory between the GPU and ARM CPU
    // ldr     sp, =0x8000
    ldr     sp, =(64 * 1024 * 1024)

    // Run the c startup function - should not return and will call kernel_main
    b       _cstartup

_inf_loop:
    b       _inf_loop
```

## armc-09-cstartup.c

```c
extern int __bss_start__;
extern int __bss_end__;

extern void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags );

void _cstartup( unsigned int r0, unsigned int r1, unsigned int r2 )
{
    /*__bss_start__ and __bss_end__ are defined in the linker script */
    int* bss = &__bss_start__;
    int* bss_end = &__bss_end__;

    /*
        Clear the BSS section

        See http://en.wikipedia.org/wiki/.bss for further information on the
            BSS section

        See https://sourceware.org/newlib/libc.html#Stubs for further
            information on the c-library stubs
    */
    while( bss < bss_end )
        *bss++ = 0;

    /* We should never return from main ... */
    kernel_main( r0, r1, r2 );

    /* ... but if we do, safely trap here */
    while(1)
    {
        /* EMPTY! */
    }
}
```

## part-2/armc-09.c

```c
#include <string.h>
#include <stdlib.h>

#include "rpi-gpio.h"

/** GPIO Register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
    int loop;
    unsigned int* counters;

    /* Set the LED GPIO pin to an output to drive the LED */
    gpio[LED_GPFSEL] |= ( 1 << LED_GPFBIT );

    /* Allocate a block of memory for counters */
    counters = malloc( 1024 * sizeof( unsigned int ) );

    /* Failed to allocate memory! */
    if( counters == NULL )
        while(1) {     LED_ON();/* Trap here */ }

    for( loop=0; loop<1024; loop++ )
        counters[loop] = 0;

    /* Never exit as there is no OS to exit to! */
    while(1)
    {
        /* Light the LED */
        LED_ON();

        for(counters[0] = 0; counters[0] < 500000; counters[0]++)
            ;

        /* Set the GPIO16 output low ( Turn OK LED on )*/
        LED_OFF();

        for(counters[1] = 0; counters[1] < 500000; counters[1]++)
            ;
    }
}
```

Build the example and program, now the counter variable is from malloc'd memory - usually malloc
is the most mysterious of the c library stubs, but as you can see from the example, you can
implement it easily and also implement it in C easily too! Of course occasionally it's necessary
to drop down to assembler to get certain register values or talk to some special hardware
features. Keep the [cheat-sheet](http://infocenter.arm.com/help/topic/com.arm.doc.qrc0001l/QRC0001_UAL.pdf)
close at hand while you're working with the C-Stubs.

## Part 3

In the next part of the bare metal tutorial, we'll add on a build system so that we're not using
a silly single command-line, and we can harness the power of a full build system for our
bare-metal environment. We'll then fall back to the Cambridge ASM tutorials and progress to
using the timer for flashing the LED rather than the incrementing loop counter we've used so far.

So, what are you waiting for? Head off to [Part3](http://www.valvers.com/embedded-linux/raspberry-pi/step03-bare-metal-programming-in-c-pt3)
now!
