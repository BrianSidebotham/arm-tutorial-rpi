
//  Part of the Raspberry-Pi Bare Metal Tutorials
//  Copyright (c) 2013, Brian Sidebotham
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//      this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

// References for RPI2 and above parts of this boot code:

// https://wiki.osdev.org/Raspberry_Pi_Bare_Bones

// Go and read and absorb this quite excellent startup code. It's really nice:
// https://github.com/LdB-ECM/Raspberry-Pi/blob/master/SD_FAT32/SmartStart32.S

.section ".text.startup"

.global _start
.global _get_stack_pointer
.global _exception_table
.global _enable_interrupts

// From the ARM ARM (Architecture Reference Manual). Make sure you get the
// ARMv5 documentation which includes the ARMv6 documentation which is the
// correct processor type for the Broadcom BCM2835. The ARMv6-M manuals
// available on the ARM website are for Cortex-M parts only and are very
// different.
//
// See ARM section A2.2 (Processor Modes)

.equ    CPSR_MODE_USER,         0x10
.equ    CPSR_MODE_FIQ,          0x11
.equ    CPSR_MODE_IRQ,          0x12
.equ    CPSR_MODE_SVR,          0x13
.equ    CPSR_MODE_ABORT,        0x17
.equ    CPSR_MODE_HYPERVISOR,   0x1A
.equ    CPSR_MODE_UNDEFINED,    0x1B
.equ    CPSR_MODE_SYSTEM,       0x1F
.equ    CPSR_MODE_MASK,         0x1F // Used to mask the Mode bit in the CPSR

// See ARM section A2.5 (Program status registers)
.equ    CPSR_IRQ_INHIBIT,       0x80
.equ    CPSR_FIQ_INHIBIT,       0x40
.equ    CPSR_THUMB,             0x20

// See ARM section B3.3
// The value can be decoded into constituent parts, but can be gathered easily by running
// cat /proc/cpuinfo when booting any of the RPi boards into Linux
.equ    MAINID_ARMV6,           0x410FB767
.equ    MAINID_ARMV7,           0x410FC073
.equ    MAINID_ARMV8,           0x410FD034

// See the following: https://github.com/raspberrypi/tools/blob/master/armstubs/armstub7.S
.equ    GIC_DISTB,              0xff841000
.equ    GICC_PMR,               0x4
.equ    GICD_CTRLR,             0x0
.equ    GICD_IGROUPR,           0x80
.equ    GIC_CPUB_offset,        0x1000

#define PRESCALER_2711	0xff800008
#define MBOX_2711	0xff8000cc

// At the start address we have a "jump table", specifically laid out to allow jump to an address that is stored in
// memory. This table must be laid out exactly as shown (including the instruction ldr pc,)
_start:
    ldr pc, _reset_h
    ldr pc, _undefined_instruction_vector_h
    ldr pc, _software_interrupt_vector_h
    ldr pc, _prefetch_abort_vector_h
    ldr pc, _data_abort_vector_h
    ldr pc, _unused_handler_h
    ldr pc, _interrupt_vector_h
    ldr pc, _fast_interrupt_vector_h

_reset_h:                           .word   _reset_
_undefined_instruction_vector_h:    .word   undefined_instruction_vector
_software_interrupt_vector_h:       .word   software_interrupt_vector
_prefetch_abort_vector_h:           .word   prefetch_abort_vector
_data_abort_vector_h:               .word   data_abort_vector
_unused_handler_h:                  .word   _reset_
_interrupt_vector_h:                .word   interrupt_vector
_fast_interrupt_vector_h:           .word   fast_interrupt_vector

_reset_:
    // Determine what mode the cpu has started up in. When we get to the very start of the ARM execution we can
    // extract the Current Program Status Register value and mask off the Mode field which is defined in the ARM
    // Architecture Reference Manual for ARMV6 ARMV7 and ARMV8 as M[4:0] - in other words the 5 lowest bits
    // Do this in a high register so as not to overwrite anything the bootloader has passed over (such as atags)
    mrs r12, CPSR
    and r12, #CPSR_MODE_MASK    // Zero all bits except the CPSR_MODE_MASK bits to be left with the mode value in r11

    // Store the CPSR start mode in a "global variable" that is accessible to all (including the C world)
    ldr r11, =_cpsr_startup_mode
    str r12, [r11]

    // Check to see if we're BCM2385 (RPI0/1) - these processors only have one core and so we can go ahead and
    // continue executing the startup code. However, all other RPI models have processors which have multiple cores.
    // When the ARM processor starts all four processors run the same code. 'tis weird, n'est pas?
    // In order to get some sanity back to our world we "park" the remaining cores and just work with one core. We're
    // not yet at the point of requiring parallel processing.

    // Skip the Hypervisor mode check and core parking when RPI0/1
    mrc p15, 0, r11, c0, c0, 0
    ldr r10, =#MAINID_ARMV6
    cmp r11, r10
    beq _setup_interrupt_table

    // Did we start up in hypervisor mode? If we didn't go ahead an park the cpus, we can then get back to SVC mode
    // later on
    cmp r12, #CPSR_MODE_HYPERVISOR
    bne _multicore_park

    // We're in hypervisor mode and we need to switch back in order to allow us to continue successfully
    mrs r12, CPSR
    bic r12, r12, #CPSR_MODE_MASK
    orr r12, r12, #(CPSR_MODE_SVR | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
    msr SPSR_cxsf, r12

    add lr, pc, #4
    .word 0xE12EF30E
    .word 0xE160006E

_multicore_park:
    // On RPI2/3 make sure all cores that are not core 0 branch off to an infinite loop to make them enter a spinlock
    // We will then only operate with core 0 and setup stack pointers and the like for core 0
    mrc p15, 0, r12, c0, c0, 5
    ands r12, #0x3
    bne _inf_loop

_setup_interrupt_table:

    mov     r0, #0x8000
    mov     r1, #0x0000
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}

    // We're going to use interrupt mode, so setup the interrupt mode
    // stack pointer which differs to the application stack pointer:
    mov r0, #(CPSR_MODE_IRQ | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
    msr cpsr_c, r0
    ldr sp, =0x7000

    // Switch back to supervisor mode (our application mode) and
    // set the stack pointer. Remember that the stack works its way
    // down memory, our heap will work it's way up from after the
    // application.
    mov r0, #(CPSR_MODE_SVR | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
    msr cpsr_c, r0
    ldr sp, =0x8000

    // The c-startup function which we never return from. This function will
    // initialise the ro data section (most things that have the const
    // declaration) and initialise the bss section variables to 0 (generally
    // known as automatics). It'll then call main, which should never return.
    bl _cstartup

    // If main does return for some reason, just catch it and stay here.
_inf_loop:
    b _inf_loop


// A 32-bit value that represents the processor mode at startup
_cpsr_startup_mode:  .word    0x0
_osc:                .word    54000000
_value:              .word    0x63fff
_mbox:               .word    MBOX_2711

_get_stack_pointer:
    // Return the stack pointer value
    str     sp, [sp]
    ldr     r0, [sp]

    // Return from the function
    mov     pc, lr


_enable_interrupts:
    mrs     r0, cpsr
    bic     r0, r0, #CPSR_IRQ_INHIBIT
    msr     cpsr_c, r0
    cpsie   i

    mov     pc, lr
