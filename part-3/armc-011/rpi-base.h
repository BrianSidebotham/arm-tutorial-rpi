/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#ifndef RPI_BASE_H
#define RPI_BASE_H

#include <stdint.h>

/* Define the peripheral base address */

#if defined( RPI0 ) || defined( RPI1 )
    #define PER_BASE       0x20000000UL
#elif defined( RPI2 ) || defined( RPI3 )
    #define PER_BASE       0x3F000000UL
#elif defined( RPI4 )
    /* This comes from the linux source code:
       https://github.com/raspberrypi/linux/blob/rpi-4.19.y/arch/arm/boot/dts/bcm2838.dtsi */
    #define PER_BASE       0xFE000000UL
#else
    #error Unknown RPI Model!
#endif


typedef volatile uint32_t rpi_reg_rw_t;
typedef volatile const uint32_t rpi_reg_ro_t;
typedef volatile uint64_t rpi_wreg_rw_t;
typedef volatile const uint64_t rpi_wreg_ro_t;

#endif
