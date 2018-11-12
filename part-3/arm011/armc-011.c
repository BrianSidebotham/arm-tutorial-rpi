/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

/*
    C-Library stubs introduced for newlib
*/

#include <string.h>
#include <stdlib.h>

#include "rpi-gpio.h"
#include "rpi-systimer.h"

/** GPIO Register set */
volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
    /* Write 1 to the GPIO init nibble in the Function Select GPIO peripheral register to enable
       the LED pin an output */
    gpio[LED_GPFSEL] |= ( 1 << LED_GPFBIT );

    /* Never exit as there is no OS to exit to! */
    while(1)
    {
        /* Wait half a second */
        RPI_WaitMicroSeconds( 500000 );
        LED_OFF();

        /* Wait half a second */
        RPI_WaitMicroSeconds( 500000 );
        LED_ON();
    }
}
