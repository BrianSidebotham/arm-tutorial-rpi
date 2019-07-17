/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#ifndef RPI_GPIO_H
#define RPI_GPIO_H

/* The base address of the GPIO peripheral (ARM Physical Address) This is defined in the
   Peripherals manual provided by Broadcom and it differs between the different Raspberry Pi
   models */

#if defined( RPI0 ) || defined( RPI1 )
    #define GPIO_BASE       0x20200000UL
#elif defined( RPI2 ) || defined( RPI3 )
    #define GPIO_BASE       0x3F200000UL
#elif defined( RPI4 )
    /* This comes from the linux source code:
       https://github.com/raspberrypi/linux/blob/rpi-4.19.y/arch/arm/boot/dts/bcm2838.dtsi */
    #define GPIO_BASE       0xFE200000UL
#else
    #error Unknown RPI Model!
#endif


/* Different Raspberry pi models have the ACT LED on different GPIO pins. The RPI3 doesn't have
   access to the ACT LED through a GPIO pin can so can't be used in this tutorial, but the RPI3B+
   does have the ACT LED on a GPIO pin again and so can be used with this tutorial! */

/* TODO: Expand this to RPi4 as necessary */
#if defined( RPI1 ) && !defined( IOBPLUS )

    /* Very early models of the RPi including the Model A or B had ACT LED available on GPIO */
    #define LED_GPFSEL      GPIO_GPFSEL1
    #define LED_GPFBIT      18
    #define LED_GPSET       GPIO_GPSET0
    #define LED_GPCLR       GPIO_GPCLR0
    #define LED_GPIO_BIT    16
#elif (defined( RPI1 ) && defined( IOBPLUS )) || defined( RPI2 ) || defined( RPI0 )

    /* The RPI1B+ and RPI2 and RPI0 (Pi Zero and ZeroW) use GPIO47 for the ACT LED */
    #define LED_GPFSEL      GPIO_GPFSEL4
    #define LED_GPFBIT      21
    #define LED_GPSET       GPIO_GPSET1
    #define LED_GPCLR       GPIO_GPCLR1
    #define LED_GPIO_BIT    15
#elif ( defined( RPI3 ) && defined( IOBPLUS ) )

    /* The RPi3B+ again made the ACT LED available on a GPIO pin (of course on yet another pin!) */
    #define LED_GPFSEL      GPIO_GPFSEL2
    #define LED_GPFBIT      27
    #define LED_GPSET       GPIO_GPSET0
    #define LED_GPCLR       GPIO_GPCLR0
    #define LED_GPIO_BIT    29

#elif defined( RPI3 )

    #error The RPI3 has an ioexpander between the ACT LED and the GPU and so cannot be used in this tutorial

#elif defined( RPI4 )
    /* The RPi4 model has the ACT LED attached to GPIO 42
       https://github.com/raspberrypi/linux/blob/rpi-4.19.y/arch/arm/boot/dts/bcm2838-rpi-4-b.dts */
    #define LED_GPFSEL      GPIO_GPFSEL4
    #define LED_GPFBIT      6
    #define LED_GPSET       GPIO_GPSET1
    #define LED_GPCLR       GPIO_GPCLR1
    #define LED_GPIO_BIT    10
#endif


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

#endif
