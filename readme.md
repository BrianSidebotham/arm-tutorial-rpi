# Raspberry-Pi Bare Metal Tutorial

![Build Status](https://travis-ci.org/BrianSidebotham/arm-tutorial-rpi.svg?branch=master)

This repository contains the code for the Raspberry-Pi bare-metal programming
in C series. The home of the tutorial and all of the articles relating to the
code is at
[valvers.com](http://www.valvers.com/open-software/raspberry-pi/step01-bare-metal-programming-in-cpt1/)

You can build on linux, windows, or MAC - all you need is the
[arm-none-eabi toolchain](https://launchpad.net/gcc-arm-embedded).

For all platforms, you can get the required toolchain used in this tutorial
from the [GCC-ARM-embedded](https://launchpad.net/gcc-arm-embedded) project -
Most OS distributions now have at least the 4.8 toolchain, however the
following will fix problems with using the 4.9 toolchain.

[Adding options -lc -specs=nosys.specs allows arm-none-eabi-gcc v4.9 for compilation](https://github.com/BrianSidebotham/arm-tutorial-rpi/issues/8)

Have fun, and remember to experiment!

## Tutorial links (online)

[Step01 - Getting Started](http://www.valvers.com/open-software/raspberry-pi/step01-bare-metal-programming-in-cpt1/)

[Step02 - C Runtime](http://www.valvers.com/open-software/raspberry-pi/step02-bare-metal-programming-in-c-pt2/)

[Step03 - Introducing CMake](http://www.valvers.com/open-software/raspberry-pi/step03-bare-metal-programming-in-c-pt3/)

[Step04 - Interrupts](http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4/)

[Step05 - Graphics(Basic)](http://www.valvers.com/open-software/raspberry-pi/step05-bare-metal-programming-in-c-pt5/)

## Some interesting links:

Original Cambridge Tutorial that inspired this tutorial series:
http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/index.html

ARM Instruction Reference:
http://infocenter.arm.com/help/topic/com.arm.doc.qrc0001l/QRC0001_UAL.pdf

GNU ARM Embedded Toolchain:
https://launchpad.net/gcc-arm-embedded

Newlib C-Library documentation:
https://sourceware.org/newlib/libc.html
