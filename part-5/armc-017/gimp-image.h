
#ifndef GIMP_IMAGE_H
#define GIMP_IMAGE_H

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel;
    unsigned char pixel_data[];
} gimp_image_t;

#endif
