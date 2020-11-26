/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2020, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

/* Including the maths library introduces a new dependency, but it's available with the compiler.
   We make changes to CMakeLists.txt to add in the maths library that's called 'm' */
#include <math.h>
#include <stdlib.h>

#include "rpi-framebuffer.h"
#include "sinewave.h"

sinewave_t* SIN_New( int amplitude, int frequency, framebuffer_info_t* fb)
{
    sinewave_t* newsine = malloc( sizeof(sinewave_t) );

    newsine->amplitude = amplitude;
    newsine->steps = (fb) ? fb->physical_width : 360;
    newsine->data = malloc( sizeof(int) * newsine->steps );

    double delta = (double)360 / ( newsine->steps / frequency );
    double deg = 0;

    for( int step = 0; step < newsine->steps; step++ )
    {
        /* Convert degrees to radians before sin function */
        newsine->data[step] = sin(deg * 0.0174533 ) * amplitude;
        deg += delta;
    }

    return newsine;
}
