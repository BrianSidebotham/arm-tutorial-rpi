/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

#include "gimp-image.h"

typedef uint32_t palette_entry_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t bytes_per_pixel;
    palette_entry_t* palette;   /**< Only valid for bytes_per_pixel < 2 */
    uint8_t* pixel_data;
    uint32_t pitch;             /**< The pitch (in bytes) per scan line of the image */
    } image_t;

extern image_t* image8_from_bitmap( const uint8_t* bitmap );
extern image_t* image16_from_gimp( gimp_image_t* gimage );

#endif
