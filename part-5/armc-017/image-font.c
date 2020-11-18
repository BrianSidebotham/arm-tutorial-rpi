
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


void font_putc( int x, int y, char c, image_font_t* font )
{
    if( font == NULL )
        return;

    int blit_addr = font->character_offsets[(int)c];
    int py;

    for( py = 0; py < font->pixel_height; py++ )
    {
        RPI_Blit( x, y + py, &font->image->pixel_data[blit_addr], font->pixel_width );
        blit_addr += font->image->pitch;
    }
}


void font_puts( int x, int y, const char* str, image_font_t* font )
{
    /* Sanitise the input parameters */
    if( ( str == NULL ) || ( font == NULL ) )
        return;

    while( *str != '\0' )
    {
        font_putc( x, y, *str, font );
        x += font->pixel_width;
        str++;
    }
}
