
/* Implements functions that allow us to negotiate, and use a graphics
   framebuffer provided by the VideoCore IV GPU using the mailbox interface. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpi-gpio.h"
#include "rpi-mailbox-interface.h"
#include "rpi-framebuffer.h"
#include "image.h"

static framebuffer_info_t framebuffer = {0};

void RPI_InitFramebuffer( int width, int height, int bpp )
{
    rpi_mailbox_property_t *mp;

    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_ALLOCATE_BUFFER );
    RPI_PropertyAddTag( TAG_SET_PHYSICAL_SIZE, width, height );
    RPI_PropertyAddTag( TAG_SET_VIRTUAL_SIZE, width, height * 2 );
    RPI_PropertyAddTag( TAG_SET_DEPTH, bpp );
    RPI_PropertyAddTag( TAG_GET_PITCH );
    RPI_PropertyAddTag( TAG_GET_PHYSICAL_SIZE );
    RPI_PropertyAddTag( TAG_GET_DEPTH );
    RPI_PropertyProcess();

    if( ( mp = RPI_PropertyGet( TAG_GET_PHYSICAL_SIZE ) ) )
    {
        framebuffer.physical_width = mp->data.buffer_32[0];
        framebuffer.physical_height = mp->data.buffer_32[1];

        printf( "Initialised Framebuffer: %dx%d ", width, height );
    }

    if( ( mp = RPI_PropertyGet( TAG_GET_VIRTUAL_SIZE ) ) )
    {
        framebuffer.virtual_width = mp->data.buffer_32[0];
        framebuffer.virtual_height = mp->data.buffer_32[1];

        printf( "Initialised Framebuffer: %dx%d ", width, height );
    }

    if( ( mp = RPI_PropertyGet( TAG_GET_DEPTH ) ) )
    {
        framebuffer.bits_per_pixel = mp->data.buffer_32[0];
        framebuffer.bytes_per_pixel = framebuffer.bits_per_pixel >> 3;
        printf( "%dbpp\r\n", framebuffer.bits_per_pixel );
    }

    if( ( mp = RPI_PropertyGet( TAG_GET_PITCH ) ) )
    {
        framebuffer.pitch = mp->data.buffer_32[0];
        printf( "Pitch: %d bytes\r\n", framebuffer.pitch );
    }

    if( ( mp = RPI_PropertyGet( TAG_ALLOCATE_BUFFER ) ) )
    {
        framebuffer.buffer = (volatile uint32_t*)(mp->data.buffer_32[0] & ~0xC0000000);
        printf( "Framebuffer address: %8.8X\r\n", (unsigned int)framebuffer.buffer );
    }

    framebuffer.buffer_size = framebuffer.pitch * framebuffer.physical_height;
}


framebuffer_info_t* RPI_GetFramebuffer( void )
{
    return &framebuffer;
}

void RPI_Blit( int x, int y, void* data, int datacount )
{
    /* Splat the data to the screen buffer */
    if ( framebuffer.bits_per_pixel == 8 )
    {
        memcpy( ((char*)framebuffer.buffer + ( y * framebuffer.physical_width ) + x ), data, datacount );
    }
    else if( framebuffer.bits_per_pixel == 16 )
    {
        memcpy( ((short*)framebuffer.buffer + ( y * framebuffer.physical_width ) + x ), data, datacount * sizeof( short ) );
    }
    else if( framebuffer.bits_per_pixel == 32 )
    {
        memcpy( ((unsigned int*)framebuffer.buffer + ( y * framebuffer.physical_width ) + x ), data, datacount * sizeof( short ) );
    }
    else
    {
        printf("RPI_Blit Unsupported bits_per_pixel(%d)", framebuffer.bits_per_pixel );
    }
}


static inline void RPI_PutPixel( int x, int y, int colour )
{
    if( framebuffer.bits_per_pixel == 8 )
    {
        *(unsigned char*)(framebuffer.buffer + ( y * framebuffer.pitch ) + x ) = colour;
    }
    else if ( framebuffer.bits_per_pixel == 16 )
    {
        *(short*)(framebuffer.buffer + ( y * framebuffer.pitch ) + x ) = colour;
    }
    else if ( framebuffer.bits_per_pixel == 16 )
    {
        *(unsigned int*)(framebuffer.buffer + ( y * framebuffer.pitch ) + x ) = colour;
    }
    else
    {
        printf("RPI_PutPixel Unsupported bits_per_pixel(%d)", framebuffer.bits_per_pixel );
    }
}


void RPI_ClearScreen( void )
{
    memset( (char*)framebuffer.buffer, 0, framebuffer.buffer_size );
}


void RPI_DrawRectangle( graphic_rectangle_t* rectangle )
{
    int px, py;
    uint32_t border[rectangle->width];
    uint32_t line[rectangle->width];
    uint16_t* short_border = (uint16_t*)&border[0];
    uint16_t* short_line = (uint16_t*)&line[0];
    uint8_t* byte_border = (uint8_t*)&border[0];
    uint8_t* byte_line = (uint8_t*)&line[0];

    for( px = 0; px < rectangle->width; px++ )
    {
        if( ( px < rectangle->border ) || ( px > ( rectangle->width - rectangle->border - 1) ) )
        {
            if( framebuffer.bits_per_pixel == 8 )
                byte_line[px] = rectangle->border_colour % 256;
            else if(framebuffer.bits_per_pixel == 16 )
                short_line[px] = rectangle->border_colour;
            else if(framebuffer.bits_per_pixel == 32 )
                line[px] = rectangle->border_colour;
        }
        else
        {
            if( framebuffer.bits_per_pixel == 8 )
                byte_line[px] = rectangle->fill_colour % 256;
            else if(framebuffer.bits_per_pixel == 16 )
                short_line[px] = rectangle->fill_colour;
            else if(framebuffer.bits_per_pixel == 32 )
                line[px] = rectangle->fill_colour;
        }

        if( framebuffer.bits_per_pixel == 8 )
            byte_border[px] = rectangle->border_colour % 256;
        else if(framebuffer.bits_per_pixel == 16 )
            short_border[px] = rectangle->border_colour;
        else if(framebuffer.bits_per_pixel == 32 )
            border[px] = rectangle->border_colour;
    }

    for( py = 0; py < rectangle->height; py++ )
    {
        if( ( py < rectangle->border ) || ( py > ( rectangle->height - rectangle->border - 1 ) ) )
            RPI_Blit( rectangle->position_x, rectangle->position_y + py, border, rectangle->width );
        else
            RPI_Blit( rectangle->position_x, rectangle->position_y + py, line, rectangle->width );
    }
}


void RPI_DrawMovingRectangle( graphic_moving_rectangle_t* mrectangle )
{
    mrectangle->rectangle.position_x += mrectangle->movement.dx;
    mrectangle->rectangle.position_y += mrectangle->movement.dy;

    if( mrectangle->rectangle.position_x <= 0 )
    {
        mrectangle->rectangle.position_x = 0;
        mrectangle->movement.dx = -mrectangle->movement.dx;
    }
    else if( ( mrectangle->rectangle.position_x + mrectangle->rectangle.width ) > ( framebuffer.physical_width - 1 ) )
    {
        mrectangle->rectangle.position_x = framebuffer.physical_width - 1 - mrectangle->rectangle.width;
        mrectangle->movement.dx = -mrectangle->movement.dx;
    }

    if( mrectangle->rectangle.position_y <= 0 )
    {
        mrectangle->rectangle.position_y = 0;
        mrectangle->movement.dy = -mrectangle->movement.dy;
    }
    else if( ( mrectangle->rectangle.position_y + mrectangle->rectangle.height ) > ( framebuffer.physical_height - 1 ) )
    {
        mrectangle->rectangle.position_y = framebuffer.physical_height - 1 - mrectangle->rectangle.height;
        mrectangle->movement.dy = -mrectangle->movement.dy;
    }

    mrectangle->rectangle.fill_colour += 1;
    mrectangle->rectangle.fill_colour %= 16;

    RPI_DrawRectangle( &mrectangle->rectangle );
}


void RPI_DrawImage( int x, int y, image_t* image )
{
    int blit_y = 0;

    /* NOTE: We only support images that have the same bits-per-pixel as the display. We do not
             fall back to a slower method. But, we could */

    if( ( RPI_GetFramebuffer()->bits_per_pixel == 16 ) && ( image->bytes_per_pixel == 2 ) )
    {
        for( blit_y = 0; blit_y < image->height; blit_y++ )
            RPI_Blit( x, y + blit_y,
                     (short*)&image->pixel_data[ blit_y * image->width * image->bytes_per_pixel ],
                      image->width );
    }
    else if( ( RPI_GetFramebuffer()->bits_per_pixel == 8 ) && ( image->bytes_per_pixel == 1 ) )
    {
        for( blit_y = 0; blit_y < image->height; blit_y++ )
            RPI_Blit( x, y + blit_y,
                      &image->pixel_data[ blit_y * image->width * image->bytes_per_pixel ],
                      image->width );
    }
    else if( ( RPI_GetFramebuffer()->bits_per_pixel == 32 ) && ( image->bytes_per_pixel == 4 ) )
    {
        for( blit_y = 0; blit_y < image->height; blit_y++ )
            RPI_Blit( x, y + blit_y,
                      &image->pixel_data[ blit_y * image->width * image->bytes_per_pixel ],
                      image->width );
    }
}
