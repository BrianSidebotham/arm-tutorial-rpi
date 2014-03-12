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

/*
    PWM Example using the System Timer for brightness control of the OK LED
*/

#include <string.h>
#include <stdlib.h>

#include "rpi-gpio.h"
#include "rpi-systimer.h"

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
    int brightness = 255;
    int speed = 16;
    int up = 0;

    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    RPI_GetGpio()->GPFSEL1 |= (1 << 18);

    /* Never exit as there is no OS to exit to! */
    while(1)
    {
        if( brightness > 0 )
        {
            /* Set the GPIO16 output high ( Turn OK LED off )*/
            RPI_GetGpio()->GPSET0 = (1 << 16);

            /* Wait half a second */
            RPI_WaitMicroSeconds( brightness );
        }

        if( ( 255 - brightness ) >= 0 )
        {
            /* Set the GPIO16 output low ( Turn OK LED on )*/
            RPI_GetGpio()->GPCLR0 = (1 << 16);

            /* Wait half a second */
            RPI_WaitMicroSeconds( 255 - brightness );
        }

        speed--;
        if( speed == 0 )
        {
            speed = 16;

            if( up )
            {
                if( brightness < 255 )
                    brightness++;

                if( brightness == 255 )
                    up = 0;
            }
            else
            {
                if( brightness )
                    brightness--;

                if( brightness == 0 )
                    up = 1;
            }
        }
    }
}
