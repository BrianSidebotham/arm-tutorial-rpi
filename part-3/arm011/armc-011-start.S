
//  Part of the Raspberry-Pi Bare Metal Tutorials
//  https://www.valvers.com/rpi/bare-metal/
//  Copyright (c) 2013, Brian Sidebotham

//  This software is licensed under the MIT License.
//  Please see the LICENSE file included with this software.

.section ".text.startup"

.global _start
.global _get_stack_pointer

_start:
    // Set the stack pointer at the end of RAM.
    // Keep it within the limits and also keep it aligned to a 32-bit
    // boundary!
    ldr     sp, =0x8000

    // The c-startup
    b       _cstartup

_inf_loop:
    b       _inf_loop
