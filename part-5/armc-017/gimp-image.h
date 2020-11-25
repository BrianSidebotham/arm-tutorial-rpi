/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#ifndef GIMP_IMAGE_H
#define GIMP_IMAGE_H

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel;
    unsigned char pixel_data[];
} gimp_image_t;

#endif
