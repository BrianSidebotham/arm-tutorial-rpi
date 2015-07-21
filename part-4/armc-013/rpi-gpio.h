/*

    Part of the Raspberry-Pi Bare Metal Tutorials
    Copyright (c) 2013, Brian Sidebotham
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
        this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef RPI_GPIO_H
#define RPI_GPIO_H

#include "rpi-base.h"

/** The base address of the GPIO peripheral (ARM Physical Address) */
#define RPI_GPIO_BASE       ( PERIPHERAL_BASE + 0x200000UL )

#if defined( RPIBPLUS ) || defined( RPI2 )
    #define LED_GPFSEL      GPFSEL4
    #define LED_GPFBIT      21
    #define LED_GPSET       GPSET1
    #define LED_GPCLR       GPCLR1
    #define LED_GPIO_BIT    15
    #define LED_ON()        do { RPI_GetGpio()->LED_GPCLR = ( 1 << LED_GPIO_BIT ); } while( 0 )
    #define LED_OFF()       do { RPI_GetGpio()->LED_GPSET = ( 1 << LED_GPIO_BIT ); } while( 0 )
#else
    #define LED_GPFSEL      GPFSEL1
    #define LED_GPFBIT      18
    #define LED_GPSET       GPSET0
    #define LED_GPCLR       GPCLR0
    #define LED_GPIO_BIT    16
    #define LED_ON()        do { RPI_GetGpio()->LED_GPSET = ( 1 << LED_GPIO_BIT ); } while( 0 )
    #define LED_OFF()       do { RPI_GetGpio()->LED_GPCLR = ( 1 << LED_GPIO_BIT ); } while( 0 )
#endif

/***/
#define RPI_GPIO_FSEL0_00_INPUT     ( 0 )
#define RPI_GPIO_FSEL0_00_OUTPUT    ( 1 )

#define RPI_GPIO_FSEL0_01_INPUT     ( 0 << 3 )
#define RPI_GPIO_FSEL0_01_OUTPUT    ( 1 << 3 )

#define RPI_GPIO_FSEL0_02_INPUT     ( 0 << 6 )
#define RPI_GPIO_FSEL0_02_OUTPUT    ( 1 << 6 )

#define RPI_GPIO_FSEL0_03_INPUT     ( 0 << 9 )
#define RPI_GPIO_FSEL0_03_OUTPUT    ( 1 << 9 )

#define RPI_GPIO_FSEL0_04_INPUT     ( 0 << 12 )
#define RPI_GPIO_FSEL0_04_OUTPUT    ( 1 << 12 )

#define RPI_GPIO_FSEL0_05_INPUT     ( 0 << 15 )
#define RPI_GPIO_FSEL0_05_OUTPUT    ( 1 << 15 )

#define RPI_GPIO_FSEL0_06_INPUT     ( 0 << 18 )
#define RPI_GPIO_FSEL0_06_OUTPUT    ( 1 << 18 )

#define RPI_GPIO_FSEL0_07_INPUT     ( 0 << 21 )
#define RPI_GPIO_FSEL0_07_OUTPUT    ( 1 << 21 )

#define RPI_GPIO_FSEL0_08_INPUT     ( 0 << 24 )
#define RPI_GPIO_FSEL0_08_OUTPUT    ( 1 << 24 )

#define RPI_GPIO_FSEL0_09_INPUT     ( 0 << 27 )
#define RPI_GPIO_FSEL0_09_OUTPUT    ( 1 << 27 )




/** The GPIO Peripheral is described in section 6 of the BCM2835 Peripherals
    documentation.

    There are 54 general-purpose I/O (GPIO) lines split into two banks. All
    GPIO pins have at least two alternative functions within BCM. The
    alternate functions are usually peripheral IO and a single peripheral
    may appear in each bank to allow flexibility on the choice of IO voltage.
    Details of alternative functions are given in section 6.2. Alternative
    Function Assignments.

    The GPIO peripheral has three dedicated interrupt lines. These lines are
    triggered by the setting of bits in the event detect status register. Each
    bank has its’ own interrupt line with the third line shared between all
    bits.

    The Alternate function table also has the pull state (pull-up/pull-down)
    which is applied after a power down. */
typedef struct {
    rpi_reg_rw_t    GPFSEL0;
    rpi_reg_rw_t    GPFSEL1;
    rpi_reg_rw_t    GPFSEL2;
    rpi_reg_rw_t    GPFSEL3;
    rpi_reg_rw_t    GPFSEL4;
    rpi_reg_rw_t    GPFSEL5;
    rpi_reg_ro_t    Reserved0;
    rpi_reg_wo_t    GPSET0;
    rpi_reg_wo_t    GPSET1;
    rpi_reg_ro_t    Reserved1;
    rpi_reg_wo_t    GPCLR0;
    rpi_reg_wo_t    GPCLR1;
    rpi_reg_ro_t    Reserved2;
    rpi_reg_wo_t    GPLEV0;
    rpi_reg_wo_t    GPLEV1;
    rpi_reg_ro_t    Reserved3;
    rpi_reg_wo_t    GPEDS0;
    rpi_reg_wo_t    GPEDS1;
    rpi_reg_ro_t    Reserved4;
    rpi_reg_wo_t    GPREN0;
    rpi_reg_wo_t    GPREN1;
    rpi_reg_ro_t    Reserved5;
    rpi_reg_wo_t    GPFEN0;
    rpi_reg_wo_t    GPFEN1;
    rpi_reg_ro_t    Reserved6;
    rpi_reg_wo_t    GPHEN0;
    rpi_reg_wo_t    GPHEN1;
    rpi_reg_ro_t    Reserved7;
    rpi_reg_wo_t    GPLEN0;
    rpi_reg_wo_t    GPLEN1;
    rpi_reg_ro_t    Reserved8;
    rpi_reg_wo_t    GPAREN0;
    rpi_reg_wo_t    GPAREN1;
    rpi_reg_ro_t    Reserved9;
    rpi_reg_wo_t    GPAFEN0;
    rpi_reg_wo_t    GPAFEN1;
    rpi_reg_ro_t    Reserved10;
    rpi_reg_wo_t    GPPUD;
    rpi_reg_wo_t    GPPUDCLK0;
    rpi_reg_wo_t    GPPUDCLK1;
    rpi_reg_ro_t    Reserved11;
} rpi_gpio_t;


extern rpi_gpio_t* RPI_GetGpio(void);
extern void RPI_GpioInit(void);

#endif
