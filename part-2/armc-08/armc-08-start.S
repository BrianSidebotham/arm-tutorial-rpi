
//  Part of the Raspberry-Pi Bare Metal Tutorials
//  https://www.valvers.com/rpi/bare-metal/
//  Copyright (c) 2013, Brian Sidebotham

//  This software is licensed under the MIT License.
//  Please see the LICENSE file included with this software.

.section ".text.startup"

.global _start

_start:
    // Set the stack pointer, which progresses downwards through memory
    // Set it at 64MB which we know our application will not crash into
    // and we also know will be available to the ARM CPU. No matter what
    // settings we use to split the memory between the GPU and ARM CPU
    ldr     sp, =0x8000

    // Run the c startup function - should not return and will call kernel_main
    b       _cstartup

_inf_loop:
    b       _inf_loop
