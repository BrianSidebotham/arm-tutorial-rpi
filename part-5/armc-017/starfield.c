/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2020, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#include "starfield.h"
#include "stars.h"
#include "rpi-framebuffer.h"

unsigned short star_colours[32] = { 0 };

static void init_starfield( void )
{
    /* Generate the colour palette */
    for( int i=0; i < 32 ; i++ )
    {
        /* In 16-bit colours, it's rrrrrggggggbbbbb MSB->LSB */
        star_colours[i] = ( i << (5+6) ) | ( i << 6 ) | i;
    }
}

void process_starfield( void )
{
    static int init = 1;

    if( init )
    {
        init_starfield();
        init = 0;
    }

    for( int i = 0; i < STARS_COUNT; i++ )
    {
        stars[i].x -= stars[i].speed << 1;

        if( ( stars[i].x >> 4 ) <= 0 )
        {
            stars[i].x = 800 << 4;
        }

        if( ( stars[i].x >> 4 ) < 800 )
        {
            RPI_PutPixel( stars[i].x >> 4, stars[i].y, star_colours[stars[i].speed] );
        }
    }
}
