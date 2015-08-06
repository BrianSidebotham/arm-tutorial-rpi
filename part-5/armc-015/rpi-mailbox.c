/*

    Part of the Raspberry-Pi Bare Metal Tutorials
    Copyright (c) 2015, Brian Sidebotham
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
        this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdint.h>

#include "rpi-gpio.h"
#include "rpi-mailbox.h"

/* Mailbox 0 mapped to it's base address */
static mailbox_t* rpiMailbox0 = (mailbox_t*)RPI_MAILBOX0_BASE;

void RPI_Mailbox0Write( mailbox0_channel_t channel, int value )
{
    /* For information about accessing mailboxes, see:
       https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes */

    /* Add the channel number into the lower 4 bits */
    value &= ~(0xF);
    value |= channel;

    /* Wait until the mailbox becomes available and then write to the mailbox
       channel */
    while( ( rpiMailbox0->Status & ARM_MS_FULL ) != 0 ) { }

    /* Write the modified value + channel number into the write register */
    rpiMailbox0->Write = value;
}


int RPI_Mailbox0Read( mailbox0_channel_t channel )
{
    /* For information about accessing mailboxes, see:
       https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes */
    int value = -1;

    /* Keep reading the register until the desired channel gives us a value */
    while( ( value & 0xF ) != channel )
    {
        /* Wait while the mailbox is empty because otherwise there's no value
           to read! */
        while( rpiMailbox0->Status & ARM_MS_EMPTY ) { }

        /* Extract the value from the Read register of the mailbox. The value
           is actually in the upper 28 bits */
        value = rpiMailbox0->Read;
    }

    /* Return just the value (the upper 28-bits) */
    return value >> 4;
}
