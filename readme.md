# Raspberry-Pi Bare Metal Tutorial

![Build Status](https://travis-ci.org/BrianSidebotham/arm-tutorial-rpi.svg?branch=master)

This repository contains the code for the Raspberry-Pi bare-metal programming
in C series. The home of the tutorial and all of the articles relating to the
code is at
[valvers.com](http://www.valvers.com/open-software/raspberry-pi/step01-bare-metal-programming-in-cpt1/)

You can build on linux - because if you're going to develop, you may as well get yourself the ability to use Linux
whether as a VM on your Windows desktop or as your main desktop.

> **NOTE**: This tutorial also used to be tested on Windows, but as I don't have any Windows installs available any
more, or indeed the time available to test everything under Windows it's dropped off the list.

## Getting Started

Clone this repository and get the toolchain and firmware.

The toolchain used for the tutorial can be fetched by the `compiler/get_compiler.sh` script which will download the
compiler locally so you don't need to install any specific packages from your Linux distro.

Similarly, the required Raspberry Pi firmware is also stored locally and can be fetched by using the
`firmware/get_firmware_repo.sh` script.

Get reading the tutorial.

Have fun, and remember to experiment!

## Tutorial links (online)

[Step01 - Getting Started](http://www.valvers.com/open-software/raspberry-pi/step01-bare-metal-programming-in-cpt1/)

[Step02 - C Runtime](http://www.valvers.com/open-software/raspberry-pi/step02-bare-metal-programming-in-c-pt2/)

[Step03 - Introducing CMake](http://www.valvers.com/open-software/raspberry-pi/step03-bare-metal-programming-in-c-pt3/)

[Step04 - Interrupts](http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4/)

[Step05 - Graphics(Basic)](http://www.valvers.com/open-software/raspberry-pi/step05-bare-metal-programming-in-c-pt5/)

## Some interesting links:

Original [Cambridge Tutorial](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/index.html) that
inspired this tutorial series.

[ARM Instruction Reference](http://infocenter.arm.com/help/topic/com.arm.doc.qrc0001l/QRC0001_UAL.pdf)

[Circle C++ Bare Metal Environment for RPI](https://github.com/rsta2/circle)

[Newlib C-Library documentation](https://sourceware.org/newlib/libc.html)
