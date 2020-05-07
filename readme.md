# Raspberry-Pi Bare Metal Tutorial

![Build Status](https://travis-ci.org/BrianSidebotham/arm-tutorial-rpi.svg?branch=master)

This repository contains the code for the Raspberry-Pi bare-metal programming
in C series. The home of the tutorial and all of the articles relating to the
code is at
[valvers.com](https://www.valvers.com/open-software/raspberry-pi/bare-metal-programming-in-c)
and [github](https://github.com/BrianSidebotham/arm-tutorial-rpi)

You can build on linux - because if you're going to develop, you may as well get yourself the ability to use Linux
whether as a VM on your Windows desktop or as your main desktop.

> **NOTE**: This tutorial also used to be tested on Windows, but as I don't have any Windows
installs available any more, or indeed the time available to test everything under Windows it's
dropped off the list.

## Getting Started

Clone this repository and get the toolchain and firmware.

The toolchain used for the tutorial can be fetched by the `compiler/get_compiler.sh` script which will download the
compiler locally so you don't need to install any specific packages from your Linux distro.

Similarly, the required Raspberry Pi firmware is also stored locally and can be fetched by using the
`firmware/get_firmware_repo.sh` script.

Get reading the tutorial.

Have fun, and remember to experiment!

## Tutorial links (online)

[Step01 - Getting Started](/part-1/readme.md)

[Step02 - C Runtime](/part-2/readme.md)

[Step03 - Introducing CMake](/part-3/readme.md)

[Step04 - Interrupts](/part-4/readme.md)

[Step05 - Graphics(Basic)](/part-5/readme.md)

## Some interesting links:

Original [Cambridge Tutorial](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/index.html) that
inspired this tutorial series.

[ARM Instruction Reference](http://infocenter.arm.com/help/topic/com.arm.doc.qrc0001l/QRC0001_UAL.pdf)

[Circle C++ Bare Metal Environment for RPI](https://github.com/rsta2/circle)

[Newlib C-Library documentation](https://sourceware.org/newlib/libc.html)
