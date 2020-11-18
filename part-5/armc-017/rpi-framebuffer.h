
#ifndef RPI_FRAMEBUFFER_H
#define RPI_FRAMEBUFFER_H

#include "image.h"

/* A structure can hold all sorts of information about our framebuffer so we don't need to keep
going through the mailbox interface in order to get information */
typedef struct {
    int physical_width;
    int physical_height;
    int virtual_width;
    int virtual_height;
    int pitch;
    int bits_per_pixel;
    int x;
    int y;
    int bytes_per_pixel;
    int virtual_offset;
    int buffer_size;
    volatile void* buffer;
} framebuffer_info_t __attribute__( ( aligned (16) ) );


typedef struct {
    char* data;     /**< The data buffer (malloc'd) */
    int data_pitch; /**< The pitch of the data (i.e. the length of each "line") */
    int data_lines; /**< The number of lines in this data */
    } bitmap_t;

typedef struct {
    int position_x;
    int position_y;
    int width;
    int height;
    int border;
    short border_colour;
    int fill;
    short fill_colour;
    bitmap_t bitmap;    /**< The bitmap of the rectangle that can be blitted to the screen */
    } graphic_rectangle_t;


typedef struct {
    int dx;
    int dy;
    } graphic_movement_t;


typedef struct {
    graphic_rectangle_t rectangle;
    graphic_movement_t movement;
    } graphic_moving_rectangle_t;


extern void RPI_InitFramebuffer( int width, int height, int bpp );
extern framebuffer_info_t* RPI_GetFramebuffer( void );
extern void RPI_ClearScreen( void );
extern void RPI_DrawRectangle( graphic_rectangle_t* rectangle );
extern void RPI_DrawMovingRectangle( graphic_moving_rectangle_t* mrectangle );
extern void RPI_DrawImage( int x, int y, image_t* image );
extern void RPI_Blit( int x, int y, void* data, int datacount );

#endif
