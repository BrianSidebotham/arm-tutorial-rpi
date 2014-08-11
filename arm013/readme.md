# Part 4 - Interrupts

In this tutorial, we're going to look at using interrupts to generate the LED flash. Interrupts are an essential ingredient in embedded programming. We're going to investigate the BCM2835 interrupt process and implement an interrupt for the ARM Timer peripheral to blink the LED. I appreciate that blinking an LED
is probably starting to get boring, but small steps are the way to learn a big system, and learning how to handle interrupts will be enough of a learning curve without having to change what we're doing at the same time. In the next tutorial we'll move away from blinking an LED.

## Reference Material

We need some reading material for this tutorial - this is how I put the tutorial together, by reading and studying the manuals available for the processor. Yes there's a lot of text and more than one manual - but that's the only way you learn!

The material that's useful:

- [http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf](ARM1176JZF-S Technical Reference Manual)
- [http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf](BCM2385 ARM Peripherals)
- [http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0406c/index.html](ARMv5 Architecture Reference Manual). This requires agreeing to an NDA to get hold of the PDF version of the document, but it's at least free and easy to get!

All of those documents, and an ARM instruction set reference are useful for this tutorial.

## The Code

The code for the tutorial is hosted on [https://github.com/](GitHub), [https://github.com/BrianSidebotham/arm-tutorial-rpi/tree/master/arm013](here). The best thing to do to get the code is to clone the [https://github.com/BrianSidebotham/arm-tutorial-rpi](arm-tutorial-rpi) repo.

Some of the code that's specific to the tutorial and differs from the last tutorial will be discussed here.

## Interrupts

Let's get straight what an interrupt is. In terms of the ARM processor we're using an interrupt is simply a type of exception. An exception in the processor causes the PC (Program Counter) to be set to a pre-defined value. This pre-defined value will cause code execution to be interrupted and for code execution to run an exception handler put the pre-defined position. At the end of this exception handler control is generally returned to the previously executing code. Exception handlers should be quick and concise as they can occur frequently and obviously take time away from the main code.

Interrupt signals are generally created by hardware. The exception we all know, possibly without realising it is the reset signal. If we strobe the reset line the PC is set to 0 and execution starts from this address. Reset is a bit special though, there's no way to return from the reset exception! All other exceptions can be returned from because the previous PC value has been saved.

Although I've stated above that when resetting a processor, execution starts at address 0, this is not always correct. For example in many processors that support bootloaders the reset value can be different so that the application code starts at address 0 and the bootloader code starts somewhere else. Upon strobing the reset line execution will start at the reset vector, which can be different to 0. The important thing here is simply the term _vector_. A vector is a value that will be loaded into the processors PC when a given condition occurs.

Each exception type has a vector, and these vectors reside next to each other in memory in what's termed the vector table. See, it's all pretty easy really! The base address of the vector table is 0. Vector tables come in a few varieties, either each vector is simply a value to be loaded into the PC to start execution at a linker-determined position, or else the vector is code that will be executed straight away without any need to do another PC load.

For the ARMv6 architecture the vector table is at memory address 0 and is organised like below, from section A2.6 of the ARM Architecture Reference Manual covering ARMv5 and ARMv6. Please note, that on the ARM documentation website all you'll see is a reference to ARMv6-M or ARMv5. ARMv6-M is for the Cortex-M range of processors which are designed to be more heavily embedded than the application processors (The Cortex-A range), so make sure you get the ARMv5 document which also covers the ARMv6 architecture (which is what the ARM1176JZF-S is).

| Exception Type  | Mode | VE | Normal Address | High Vector Address |
|:---------------:|:----:|:--:|:--------------:|:-------------------:|
| Reset                     | Supervisor    |   | 0x00 | 0xFFFF0000 |
| Undefined Instruction     | Undefined     |   | 0x04 | 0xFFFF0004 |
| Software Interrupt (SWI)  | Supervisor    |   | 0x08 | 0xFFFF0008 |
| Prefetch Abort            | Abort         |   | 0x0C | 0xFFFF000C |
| Data Abort                | Abort         |   | 0x10 | 0xFFFF0010 |
| IRQ (Interrupt)           | IRQ           | 0 | 0x18 | 0xFFFF0018 |
| IRQ (Interrupt)           | IRQ           | 1 | Implementation Defined |
| FIQ (Fast Interrupt)      | FIQ           | 0 | 0x1C | 0xFFFF001C |
| FIQ (Fast Interrupt)      | FIQ           | 1 | Implementation Defined |

As you can see, as we've been discussing, the reset vector is at address 0. The next vector is for us to deal with an undefined instruction exception, an unlikely scenario, but something we can at least trap and debug at some point. Following that we have the Software Interrupt, Pre-fetch abort, Data Abort, IRQ (Interrupt) and FIQ (Fast Interrupt) exception vectors. The vectors are sequential except between Data Abort and IRQ where there is an 4-byte gap. The note above the table in the document reads:

NOTE: The normal vector at address 0x00000014 and the high vector at address 0xFFFF0014 are reserved for future expansion.

Vectors are only 4-bytes (one 32-bit instruction) apart. We know therefore the reset vector is simply going to be a branch instruction to the start of our actual code so that the undefined instruction exception can be implemented too. In fact, each of these vectors is simply a branch instruction to a handler somewhere else in memory.

The Normal Address is what we're interested in on the Raspberry-Pi. The High Vector Address is selected in hardware, and that hardware doesn't exist, so the vectors reside at the normal addresses. It's useful, as I've said before to have another place to start executing code if we need a different application installed such as another bootloader or something. We can ignore the High Vector Addresses.

Then we have the Mode column - this is going to be very important. If we look at the vector table, the mode listed for the reset vector is Supervisor.

## Processor Modes

Further back in section A2.2 of the ARM Architecture Reference Manual the documentation covers processors modes. When the processor is reset it is running in Supervisor mode, which is known as a privileged mode. From this operating mode we can write to most of the processors registers and can also change the processors current mode. The only mode that cannot change modes is User - this is designed to execute applications, whereas the privileged modes are meant for operating system tasks.

There is basically a mode per exception. As was mentioned previously we start at the reset vector in Supervisor mode, so far all of our code has operating in that single mode. However, when an exception occurs the processor changes mode to the exception-specific mode. The mode can also be changed in software too if necessary.

The next section of the Architecture Reference Manual describes the registers available. This is again, another important section. Look at Figure A2-1 (Register Organization) and you'll see something interesting...

![ARMv5 Figure A2-1](https://github.com/BrianSidebotham/arm-tutorial-rpi/raw/master/arm013/images/armv5-figure-a2-1-register-organization.png)

As seen by the small icon and note at the bottom of the table, some of the registers are mode-specific. This can be useful, for example in the Fast Interrupt exception a lot of registers have been replaced by mode-specific registers. This means that we can use these registers without fear of altering the behaviour of code that was operating in User or Supervisor mode before the Fast Interrupt Exception occurred.

Normally, the first thing we would need to do in an interrupt handler would be to save the registers we're going to use and the last thing we'd do in an interrupt handler would be to restore those registers values to what they were before the interrupt handler executed. The code like this at the start of a function or interrupt handler is called the prologue and the code at the end of the function or handler is known as the epilogue.

The shadow register implementation means that many times there's no need to save and restore registers in a Fast Interrupt service routine.

## Coding Interrupt Handlers

Providing interrupt handlers can be a bit tricky. It's generally entirely non-portable and you'll have to read the manual for your toolchain in order to understand how to write them. This generally involves informing the compiler that a function you declare should be used to handle a type of interrupt. The toolchain can them fill the vector table with the necessary information. The vector table is usually present in the linker script which defines the memory layout of the target device. because normally these vectors are read-only.

For gcc and ld we can look at the gcc manual on function attributes relating to ARM. [https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html](https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html)

It reads:

    Use this attribute on the ARC, ARM, AVR, CR16, ... ports to indicate that the specified function is an interrupt handler. The compiler generates function entry and exit sequences suitable for use in an interrupt handler when this attribute is present.

    Note, for the ARM, you can specify the kind of interrupt to be handled by adding an optional parameter to the interrupt attribute like this:

        void f () __attribute__ ((interrupt ("IRQ")));

    Permissible values for this parameter are: IRQ, FIQ, SWI, ABORT and UNDEF.

The parameter options align well with the vector table we got from the manual. Reset is an ABORT type, and everything else has a parameter available.

Therefore to write a very basic "undefined instruction" handler we can define declare and implement our handler like this:

    /**
        @brief The undefined instruction interrupt handler

        If an undefined instruction is encountered, the CPU will start
        executing this function. Just trap here as a debug solution.
    */
    void __attribute__((interrupt("UNDEF"))) undefined_instruction_vector(void)
    {
        while( 1 )
        {
            /* Do Nothing! */
        }
    }

While it looks like this is useless code, it is actually pretty useful. We can set a breakpoint on the while(1) and debug whilst looking out for this breakpoint to trip. Usually then we can do some stack unwinding (manually!) to find where the source of the undefined instruction.

## Coding Vector Tables

A strange title that, Coding Vector Tables. With many embedded processors there's no coding or runtime setup of vector tables - instead the vector table is defined in the linker script and the toolchain fills in those locations with the address or jump instruction necessary to execute the relevant handler.

The Raspberry-Pi vector table is extremely small, and it can be like this because some extra work is required in the Interrupt handler compared to more embedded processors such as the Cortex M range. No matter what the Interrupt source is, the same function is called. Inside the Interrupt handler we must work out what the source of the interrupt was before handling that interrupt. Essentially you end up with a large switch structure inside the interrupt handler. You can of course call other functions from the interrupt handler, but again think about the cost of the prologue and epilogue before you do!

When the raspberry-pi starts running our code, as we've discovered earlier in the turotials it loads the code from the SD card to RAM at address 0x00008000 and then starts executing the code. So how can we possibly code the vector table and get the correct jump instructions into the vector table?

Well, everything is in RAM and that's the key - this is read/write memory and in supervisor mode we can write to any location we want. So at runtime when our code is being executed we can overwrite the vector table at runtime. All we need is a vector table to write.

Let's have a look at what needs to be done to implement the vector table. It's easiest if we drop down to assembler to do this. It really could do with being done as the very first thing in our code anyway so it's setup right from the start.

Here is some modified assembler for our _statup: label which is where our linker knows we want to start execution. The linker makes sure this is at the start of our binary:

At fist, I thought this was going to be easy - we just hard-code some values into a table and copy that table to the start of RAM where the vector table resides. Here's some initial code:

    _start:
        ldr pc, =_reset_
        ldr pc, =undefined_instruction_vector
        ldr pc, =software_interrupt_vector
        ldr pc, =prefetch_abort_vector
        ldr pc, =_reset_
        ldr pc, =_reset_
        ldr pc, =interrupt_vector
        ldr pc, =fast_interrupt_vector

    _reset_:
        // Copy the vector table to the active table at 0x00000000
        ldr     r0, =_start
        ldr     r1, #0x0000
        ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
        stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}

There are only a few assembler instructions here, so don't panic! The labels refer to C functions that have been implemented with the correct attributes for each exception type. See the file rpi-interrupts.c to see these C functions.

The Loading of addresses to a register with LDR is documented [here](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473c/Bgbbfgia.html)

_reset_ and undefined_instruction_vector, etc. are the labels used as the function addresses. So we have a table at the very start of our code. A neat side-effect of having this table right at the start of our code is that the first thing we do is run the reset vector code which jumps to the _reset_ label and starts the execution of our code, thus jumping the vector table we don't want it to try and execute. Excellent!

The ldmia and stmia instructions can load and store 32 bytes of data at a time, so are quick at shifting data about. Each register can load a 4-byte (32-bit) value, so using eight registers lets us move 32-bytes, and there are 8 32-bit vectors in our table, so one load and one store instruction later we've moved the whole of our vector table to 0x00000000.

See the documentation for this on the [ARM website](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0068b/BABEFCIB.html)

But, before we go too much further, let me show you why this doesn't work!

### It's all relative

Compilation of the vector table is successful, but as always just a successful compilation doesn't get you a working application. Earlier in the tutorials I introduced the online disassembler - a great tool, but there's something else in our toolchain's toolbag already, objdump!

    arm-none-eabi-objdump --help

That's your friend, and even more friendly is running this from the scripts directory of the arm013/scripts tutorial folder once you've built the tutorial:

    arm-none-eabi-objdump -S arm-013 > arm-013.disasm

This tells objdump to disassemble the executable and we then redirect objdumps output to the arm-013.diasm file which we can then look at in a text editor. Here's what we get at the start of that file with the vector table implementation I described above:

    Disassembly of section .text:

    00008000 <_start>:
        8000:   e59ff064    ldr pc, [pc, #100]  ; 806c <_enable_interrupts+0x10>
        8004:   e59ff064    ldr pc, [pc, #100]  ; 8070 <_enable_interrupts+0x14>
        8008:   e59ff064    ldr pc, [pc, #100]  ; 8074 <_enable_interrupts+0x18>
        800c:   e59ff064    ldr pc, [pc, #100]  ; 8078 <_enable_interrupts+0x1c>
        8010:   e59ff054    ldr pc, [pc, #84]   ; 806c <_enable_interrupts+0x10>
        8014:   e59ff050    ldr pc, [pc, #80]   ; 806c <_enable_interrupts+0x10>
        8018:   e59ff05c    ldr pc, [pc, #92]   ; 807c <_enable_interrupts+0x20>
        801c:   e59ff05c    ldr pc, [pc, #92]   ; 8080 <_enable_interrupts+0x24>

    00008020 <_reset_>:
        8020:   e3a00902    mov r0, #32768  ; 0x8000

Hmmm, well that doesn't look right, what we'd expect from the first line is an equivalent of ldr pc, #0x8020 which is the _reset_ label is. The toolchain appears to be loading a value far away from where we'd expect. Further down the file we can find the location 0x806C:

    0000805c <_enable_interrupts>:
        805c:   e10f0000    mrs r0, CPSR
        8060:   e3c00080    bic r0, r0, #128    ; 0x80
        8064:   e121f000    msr CPSR_c, r0
        8068:   e1a0f00e    mov pc, lr
        806c:   00008020    .word   0x00008020
        8070:   000085e8    .word   0x000085e8
        8074:   000085f4    .word   0x000085f4
        8078:   00008600    .word   0x00008600
        807c:   00008628    .word   0x00008628
        8080:   0000869c    .word   0x0000869c

Some light at the end of the tunnel, the compiler has inserted some constants which are being loaded into the PC so actually, loading the PC with the value at PC + 0x6C is correct for the vector because the value at that location is 0x8020 which is the location of the _reset_ label.

Again, we think this could work after all. But of course, it doesn't. The first part of our code now moves the code from 0x8000 to 0x0000 and the constants have not moved from 0x806C to 0x006C; Neither can we move these constants easily either because the compiler inserts these at the next available space, there's no position we can predict.

The answer is to keep everything relative, but to create some constants of our own. We can keep these constants relative to the LDR PC instruction so that the relative offset still works. We need to create a table of constant addresses after the vector table loads. This way we can copy both tables and the constants will still be in the same position relative to the LDR instructions.

Let's set that up:

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

        mov     r0, #0x8000
        mov     r1, #0x0000
        ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
        stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
        ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
        stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}

Note that we've now had to double the amount of data we copy so that we get the constants copied along with the relative PC loads of those constants. This feels like a bit of a cludge but as objdump will show us, this gets the job done and relocates the vector table as required.

The vector table now references the labels directly suffixed to the vector table itself and so the relative position remains constant so long as both are copied as-is.

The output of objdump -S shows us the workable solution:

    Disassembly of section .text:

    00008000 <_start>:
        8000:   e59ff018    ldr pc, [pc, #24]   ; 8020 <_reset_h>
        8004:   e59ff018    ldr pc, [pc, #24]   ; 8024 <_undefined_instruction_vector_h>
        8008:   e59ff018    ldr pc, [pc, #24]   ; 8028 <_software_interrupt_vector_h>
        800c:   e59ff018    ldr pc, [pc, #24]   ; 802c <_prefetch_abort_vector_h>
        8010:   e59ff018    ldr pc, [pc, #24]   ; 8030 <_data_abort_vector_h>
        8014:   e59ff018    ldr pc, [pc, #24]   ; 8034 <_unused_handler_h>
        8018:   e59ff018    ldr pc, [pc, #24]   ; 8038 <_interrupt_vector_h>
        801c:   e59ff018    ldr pc, [pc, #24]   ; 803c <_fast_interrupt_vector_h>

    00008020 <_reset_h>:
        8020:   00008040    .word   0x00008040

    00008024 <_undefined_instruction_vector_h>:
        8024:   000085f8    .word   0x000085f8

    00008028 <_software_interrupt_vector_h>:
        8028:   00008604    .word   0x00008604

    0000802c <_prefetch_abort_vector_h>:
        802c:   00008610    .word   0x00008610

    00008030 <_data_abort_vector_h>:
        8030:   00008624    .word   0x00008624

    00008034 <_unused_handler_h>:
        8034:   00008040    .word   0x00008040

    00008038 <_interrupt_vector_h>:
        8038:   00008638    .word   0x00008638

    0000803c <_fast_interrupt_vector_h>:
        803c:   000086ac    .word   0x000086ac

    00008040 <_reset_>:
        8040:   e3a00902    mov r0, #32768  ; 0x8000

You can go ahead and see that the arm013 tutorial is in fact using this solution to provide the exception vector table at run time.

## The Interrupt Controller

TODO: Finish this section and then hopefully the tutorial is complete!


