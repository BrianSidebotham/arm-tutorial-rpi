
#ifndef IMAGE_FONT_H
#define IMAGE_FONT_H

#include "image.h"

/** @brief Define an image based font */
typedef struct {
    /** @brief The width of each character in pixels */
    int pixel_height;

    /** @brief The height of each character in pixels */
    int pixel_width;

    /** @brief There is a character offset for each ASCII character, although of course many of
        them will be set to a known "missing" character */
    int character_offsets[128];

    /** @brief The image data for the font */
    image_t* image;

    } image_font_t;

/**
    @brief Creates a new font from an image by providing blittable characters
*/
extern image_font_t* font_from_image( int width, int height, image_t* image, char unknown );
extern void font_puts( int x, int y, const char* str, image_font_t* font );
extern void font_putc( int x, int y, char c, image_font_t* font );

#endif
