/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2020, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#ifndef SINEWAVE_H
#define SINEWAVE_H

#include "rpi-framebuffer.h"

typedef struct {
    int steps;
    int amplitude;
    int* data;
    } sinewave_t;

extern sinewave_t* SIN_New( int amplitude, int frequency, framebuffer_info_t* fb );

#endif
