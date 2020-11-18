
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gimp-image.h"
#include "image.h"

#define LE32(x) ( (x)[0] | ( (x)[1] << 8 ) | ( (x)[2] << 16 ) | ( (x)[3] << 24 ) )
#define BE32(x) ( (x)[3] | ( (x)[2] << 8 ) | ( (x)[1] << 16 ) | ( (x)[0] << 24 ) )

#define LE16(x) ( (x)[0] | ( (x)[1] << 8 ) )
#define BE16(x) ( (x)[1] | ( (x)[0] << 8 ) )

#define BGR0(x) ( ( (x)[0] << 16 ) | ( (x)[1] << 8 ) | ( (x)[2] ) | ( 0xA0 << 24 ) )

image_t* image8_from_bitmap( const uint8_t* bitmap )
{
    image_t* result;
    uint32_t pixel_data_offset;
    uint32_t pixel_data_size;
    uint32_t header_size;
    uint32_t palette_size;
    uint16_t bpp;
    int px;
    int py;

    /* We only support converting certain bitmap types */
    if( !bitmap || ( bitmap[0] != 'B' ) || ( bitmap[1] != 'M' ) )
        return NULL;

    pixel_data_offset = LE32( &bitmap[10] );
    header_size = LE32( &bitmap[14] );

    /* Get the meta info for the bitmap data */
    switch( header_size )
    {
        case 40:
        case 52:
        case 56:
        case 108:
        case 124:
            break;

        default:
            /* Unsupported bitmaps */
            return NULL;
            break;
    }

    /* The total bitmap data size */
    result = malloc( sizeof( image_t ) );

    /* Get the image size */
    result->width = LE32( &bitmap[18] );
    result->height = LE32( &bitmap[22] );

    /* The resultant image is going to be 1 byte per pixel */
    bpp = LE16( &bitmap[28] );
    result->bytes_per_pixel = 1;
    result->palette = NULL;

    /* No compressions are supported */
    if( LE32( &bitmap[30] ) != 0 )
    {
        printf( "Unsupported compression of BMP image!\r\n" );
        return NULL;
    }

    palette_size = LE32( &bitmap[46] );

    printf( "IMAGE: result->width :%d\r\n", (int)result->width );
    printf( "IMAGE: result->height:%d\r\n", (int)result->height );
    printf( "IMAGE: bpp           :%d\r\n", (int)bpp );
    printf( "IMAGE: palette_size  :%d\r\n", (int)palette_size );

    /* Only support 16 colour bitmaps at the moment, other blocks would
       be easy enough to support too though! */
    if( ( palette_size == 0 ) || ( bpp != 4 ) )
    {
        printf( "No palette in image, or unsupported bpp in image!\r\n" );
        return NULL;
    }

    if( palette_size > 0 )
    {
        result->palette = malloc( sizeof( palette_entry_t ) * 256 );

        /* Extract the palette if there is one */
        for( px = 0; px < palette_size; px++ )
            result->palette[px] = BGR0( &bitmap[ 14 + header_size + (px * sizeof( palette_entry_t ) ) ] );
    }

    /* The output pixel buffer is 1 byte per pixel and therefore the size
       is simply the product of the width and height of the image */
    pixel_data_size = result->width * result->height;
    result->pixel_data = malloc( pixel_data_size );

    /* 16-colour bitmap */
    if( bpp == 4 )
    {
        /* Nibble storage for each pixel in the source bitmap so each pixel
           row is width / 2 bytes. Raster order of bitmaps is the reverse of
           everyone else in the world, so we have to perform a y-flip! */
        for( py = 0; py < pixel_data_size; py += result->width )
        {
            for( px = 0; px < result->width; px += 2 )
            {
                result->pixel_data[px + ( pixel_data_size - py - result->width )   ] =   bitmap[pixel_data_offset + (px/2) + (py/2)] & 0xF;
                result->pixel_data[px + ( pixel_data_size - py - result->width ) + 1] = ( bitmap[pixel_data_offset + (px/2) + (py/2)] >> 4 ) & 0xF;
            }
        }

        /* Calculate the pitch of the image */
        result->pitch = result->bytes_per_pixel * result->width;

        return result;
    }
    else
    {
        printf( "Unsupported bitmap input format!\r\n" );
    }

    return NULL;
}

image_t* image16_from_gimp( gimp_image_t* gimage )
{
    image_t* result;

    /* We only support data copy at the moment! */
    if( gimage->bytes_per_pixel != 2 )
        return NULL;

    result = malloc( sizeof( image_t ) );
    result->width = gimage->width;
    result->height = gimage->height;
    result->bytes_per_pixel = 2;
    result->pitch = result->bytes_per_pixel * result->width;
    result->palette = NULL;
    result->pixel_data = malloc( result->width * result->height * result->bytes_per_pixel );

    memcpy( result->pixel_data, gimage->pixel_data, result->width * result->height * result->bytes_per_pixel );

    return result;
}
