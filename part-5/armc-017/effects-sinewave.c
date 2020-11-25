/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

/* Implement the ubiquitous sinewave effect used so many times thoughout the 90s in the demoscene
   and bootloader productions */
#include <stdlib.h>

#include "sinewave.h"
#include "effects.h"

static int sin_vertical_blit_y_processor(int x, sinewave_effect_t* fx)
{
    return fx->sinewave->data[( x + fx->index ) % fx->sinewave->steps];
}

sinewave_effect_t* FX_NewSine( sinewave_settings_t settings )
{
    sinewave_effect_t* fx = malloc( sizeof(sinewave_effect_t) );
    fx->effect.effect_type = TEXT_EFFECT_SINEWAVE;
    fx->effect.vertical_blit_y_processor = sin_vertical_blit_y_processor;
    fx->index = 0;

    fx->settings.amplitude = settings.amplitude;
    fx->settings.fb = settings.fb;
    fx->settings.frequency = settings.frequency;
    fx->settings.speed = settings.speed;

    fx->sinewave = SIN_New( settings.amplitude, settings.frequency, settings.fb );
    return fx;
}

void FX_AnimateSine( sinewave_effect_t* fx )
{
    if( fx == NULL )
        return;

    fx->index += fx->settings.speed;
}
