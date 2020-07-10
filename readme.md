# Raspberry-Pi Bare Metal Tutorial

![](https://github.com/BrianSidebotham/arm-tutorial-rpi/workflows/Build/badge.svg) ![](https://github.com/BrianSidebotham/arm-tutorial-rpi/workflows/Markdown%20Lint/badge.svg)

This repository contains the code for the Raspberry-Pi bare-metal programming
in C series. The home of the tutorial and all of the articles relating to the
code is at
[valvers.com](https://www.valvers.com/open-software/raspberry-pi/bare-metal-programming-in-c)
and [github](https://github.com/BrianSidebotham/arm-tutorial-rpi)

The tutorial only supports a Linux host for compiling the tutorial code.

## Quickstart

```sh
git clone git@github.com:BrianSidebotham/arm-tutorial-rpi.git
cd arm-tutorial-rpi
```

In order to use the tutorial-supplied compiler:

```sh
compiler/get_compiler.sh
```

Get the RPI Firmware that will be required for compiled SD Card images:

```sh
firmware/get_firmware_repo.sh
```

> **NOTE**: This tutorial also used to be tested on Windows, but as I don't have any Windows
installs available any more, or indeed the time available to test everything under Windows it's
dropped off the list.

_Get reading the tutorial. Have fun, and remember to experiment!_

## Dependencies

The tutorial has a few dependencies which you might want to install before you get started:

- cmake (Used for configuring Makefiles)
- mtools (Tools for dealing with file-based FAT32 systems)

## Tutorial links (Github)

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
