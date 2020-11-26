/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#ifndef EFFECTS_H
#define EFFECTS_H

#include "rpi-framebuffer.h"
#include "sinewave.h"

typedef enum {
    UNKNOWN = 0,
    TEXT_EFFECT_SINEWAVE
    } effect_t;

typedef struct effect_info_t effect_info_t;

struct effect_info_t {
    effect_t effect_type;

    /* An effects process that will produce a new y coordinate based on various inputs. The input
       to this function call is a reference to the effect. This means that all context is
        self-contained */
    int (*vertical_blit_y_processor)(int x, effect_info_t* fx);
    };

typedef struct {
    int amplitude;
    int frequency;
    int speed;
    framebuffer_info_t* fb;
    } sinewave_settings_t;

typedef struct {
    effect_info_t effect;
    sinewave_settings_t settings;
    int index;
    sinewave_t* sinewave;
    } sinewave_effect_t;

extern sinewave_effect_t* FX_NewSine( sinewave_settings_t settings );
extern void FX_AnimateSine( sinewave_effect_t* fx );

#endif
