/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#ifndef IMAGE_FONT_H
#define IMAGE_FONT_H

#include "effects.h"
#include "image.h"

/** @brief Define an image based font */
typedef struct {
    /** @brief The width of each character in pixels */
    int pixel_height;

    /** @brief The height of each character in pixels */
    int pixel_width;

    /** @brief The data width of the font so the next data row can be calculated more easily */
    int data_width;

    /** @brief There is a character offset for each ASCII character, although of course many of
        them will be set to a known "missing" character */
    int character_offsets[128];

    /** @brief The image data for the font */
    image_t* image;

    } image_font_t;

/**
    @brief Creates a new font from an image by providing blittable characters
*/

extern int sinewave_process(int x, int y, int index, int amplitude );
extern image_font_t* font_from_image( int width, int height, image_t* image, char unknown );
extern void font_puts( int x, int y, const char* str, image_font_t* font, effect_info_t* effect );

#endif
