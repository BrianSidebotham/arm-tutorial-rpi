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

/* Peripheral base addresses - gleaned from the Linux source code (Device Tree) */
#if defined( RPI0 ) || defined( RPI1 )
    #define PERIPHERAL_BASE       (0x20000000UL)
#elif defined( RPI2 ) || defined( RPI3 )
    #define PERIPHERAL_BASE       (0x3F000000UL)
#elif defined( RPI4 )
    #define PERIPHERAL_BASE       (0xFE000000UL)
    #define GIC400_BASE           (0xFF840000UL)
#else
    #error Unknown RPI Model!
#endif

/* System Frequencies From:
   https://www.raspberrypi.org/documentation/configuration/config-txt/overclocking.md

   Except for the RPI4 where the data is incorrect and core_freq actually starts off at 200MHz as
   noted on a Github issue: https://github.com/raspberrypi/linux/issues/3381#issuecomment-568546439

   So hard to navigate the documentation bread crumbs coming from the RPi foundation! Plus, the
   starting frequencies are not necessarily those listed on that page!
   */
#if defined( RPI0 ) || defined( RPI1 ) || defined ( RPI2 ) || defined( RPI3 )
#define SYSFREQ (250000000UL)
#elif defined( RPI4 )
#define SYSFREQ (200000000UL)
#else
    #error Unknown RPI Model!
#endif

typedef volatile uint32_t rpi_reg_rw_t;
typedef volatile const uint32_t rpi_reg_ro_t;
typedef volatile uint32_t rpi_reg_wo_t;

typedef volatile uint64_t rpi_wreg_rw_t;
typedef volatile const uint64_t rpi_wreg_ro_t;

#endif
