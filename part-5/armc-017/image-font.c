/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rpi-framebuffer.h"
#include "image.h"
#include "image-font.h"

image_font_t* font_from_image( int width, int height, image_t* image, char unknown )
{
    image_font_t* font;
    int index;

    if( image == NULL )
        return NULL;

    font = malloc( sizeof( image_font_t ) );
    font->pixel_width = width;
    font->pixel_height = height;
    font->image = image;

    /* The first row of font characters must be A through K */
    for( index = 0; index < 11; index++ )
    {
        font->character_offsets[index + (unsigned char)'A'] = font->pixel_width * index * image->bytes_per_pixel;
        font->character_offsets[index + (unsigned char)'a'] = font->character_offsets[index + (unsigned char)'A'];
    }

    for( index = 0; index < 11; index++ )
    {
        font->character_offsets[index + (unsigned char)'L'] = font->pixel_width * index * image->bytes_per_pixel;
        font->character_offsets[index + (unsigned char)'L'] += font->image->pitch * font->pixel_height;
        font->character_offsets[index + (unsigned char)'l'] = font->character_offsets[index + (unsigned char)'L'];
    }

    for( index = 0; index < 4; index++ )
    {
        font->character_offsets[index + (unsigned char)'W'] = font->pixel_width * index * image->bytes_per_pixel;
        font->character_offsets[index + (unsigned char)'W'] += font->image->pitch * font->pixel_height * 2;
        font->character_offsets[index + (unsigned char)'w'] = font->character_offsets[index + (unsigned char)'W'];
    }

    /* Having done the basic alphabet we should get the other
       characters we're interested in */

    font->character_offsets[ (unsigned char)'!'] = font->pixel_width * 4 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'!'] += font->image->pitch * font->pixel_height * 2;

    font->character_offsets[ (unsigned char)'('] = font->pixel_width * 6 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'('] += font->image->pitch * font->pixel_height * 2;

    font->character_offsets[ (unsigned char)')'] = font->pixel_width * 7 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)')'] += font->image->pitch * font->pixel_height * 2;

    font->character_offsets[ (unsigned char)'-'] = font->pixel_width * 8 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'-'] += font->image->pitch * font->pixel_height * 2;

    font->character_offsets[ (unsigned char)'+'] = font->pixel_width * 9 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'+'] += font->image->pitch * font->pixel_height * 2;

    font->character_offsets[ (unsigned char)':'] = font->pixel_width * 10 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)':'] += font->image->pitch * font->pixel_height * 2;

    font->character_offsets[ (unsigned char)'.'] = font->pixel_width * 0 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'.'] += font->image->pitch * font->pixel_height * 3;

    font->character_offsets[ (unsigned char)','] = font->pixel_width * 1 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)','] += font->image->pitch * font->pixel_height * 3;

    font->character_offsets[ (unsigned char)'?'] = font->pixel_width * 2 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'?'] += font->image->pitch * font->pixel_height * 3;

    font->character_offsets[ (unsigned char)'/'] = font->pixel_width * 3 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'/'] += font->image->pitch * font->pixel_height * 3;

    font->character_offsets[ (unsigned char)'#'] = font->pixel_width * 4 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)'#'] += font->image->pitch * font->pixel_height * 3;

    font->character_offsets[ (unsigned char)' '] = font->pixel_width * 10 * image->bytes_per_pixel;
    font->character_offsets[ (unsigned char)' '] += font->image->pitch * font->pixel_height * 3;

    return font;
}


void _font_putc( int x, int y, char c, image_font_t* font, effect_info_t* effect )
{
    int blit_addr = font->character_offsets[(int)c];

    if( effect == NULL )
    {
        for( int py = 0; py < font->pixel_height; py++ )
        {
            RPI_Blit( x, y + py, &font->image->pixel_data[blit_addr], font->pixel_width );
            blit_addr += font->image->pitch;
        }
    }
    else
    {
        /* Some effects (like sinewave scrolling) result in a different y coord per x coord.
           This means we need to use the (much slower) veritical blit function */
        for( int px = 0; px < font->pixel_width; px++ )
        {
            int py = ( effect && effect->vertical_blit_y_processor ) ? effect->vertical_blit_y_processor(x + px, effect) : 0;
            RPI_BlitV(x + px, y + py, &font->image->pixel_data[blit_addr + ( px << 1 )], font->pixel_height, font->image->pitch );
        }
    }
}


void font_puts( int x, int y, const char* str, image_font_t* font, effect_info_t* effect )
{
    /* Sanitise the input parameters */
    if( ( str == NULL ) || ( font == NULL ) )
        return;

    while( *str != '\0' )
    {
        _font_putc( x, y, *str, font, effect );
        x += font->pixel_width;
        str++;
    }
}
