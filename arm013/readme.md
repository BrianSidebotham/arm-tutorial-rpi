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
| Reset                     | Supervisor    | 0x00 | 0xFFFF0000 |
| Undefined Instruction     | Undefined     | 0x04 | 0xFFFF0004 |
| Software Interrupt (SWI)  | Supervisor    | 0x08 | 0xFFFF0008 |
| Prefetch Abort            | Abort         | 0x0C | 0xFFFF000C |
| Data Abort                | Abort         | 0x10 | 0xFFFF0010 | 
| IRQ (Interrupt)           | IRQ           | 0x18 | 0xFFFF0018 |
| FIQ (Fast Interrupt)      | FIQ           | 0x1C | 0xFFFF001C |

As you can see, as we've been discussing, the reset vector is at address 0, the next vector is somewhere for us to deal with an undefined instruction exception, an unlikely scenario, but something we can at least trap and debug at some point. However, the vectors are only 4-bytes (one 32-bit instruction) apart so we know, the reset vector is simply going to be a branch instruction to the start of our actual code so that the undefined instruction exception can be implemented too. In fact, each of these vectors are simply a branch instruction to a handler somewhere else in memory.

The High Vector Address is selected in hardware, and that hardware doesn't exist, so the vectors do reside at the normal addresses. It's useful, as I've said before to have another place to start executing code if we need a different application installed such as a bootloader or something. We can ignore these addresses however.

Then we have the Mode column - this is going to be very important. If we look at the vector table, the mode listed for the reset vector is Supervisor. 

## Processor Modes

Further back in section A2.2 of the ARM Architecture Reference Manual the documentation covers processors modes. When the processor is reset it is running in Supervisor mode, which is known as a privileged mode. From this operating mode we can write to most of the processors registers and can also change the processors current mode. The only mode that cannot change modes is User - this is designed to execute applications, whereas the privileged modes are meant for operating system tasks.

There is basically a mode per exception. As was mentioned previously we start at the reset vector in Supervisor mode, so far all of our code has operating in that single mode. However, when an exception occurs the processor changes mode to the exception-specific mode. The mode can also be changed in software too if necessary.

The next section of the Architecture Reference Manual describes the registers available. This is again, another important section. Look at Figure A2-1 (Register Organization) and you'll see something interesting...

(Insert Image!)

As seen by the small icon and note at the bottom of the table, some of the registers are mode-specific. This can be useful, for example in the Fast Interrupt exception a lot of registers have been replaced by mode-specific registers. This means that we can use these registers without fear of altering the behaviour of code that was operating in User or Supervisor mode before the Fast Interrupt Exception occurred.

Normally, the first thing we would need to do in an interrupt handler would be to save the register we're going to use and the last thing we'd do in an interrupt handler would be to restore those registers values to what they were before the interrupt handler executed. The code like this at the start of a function or interrupt handler is called the prologue and the code at the end of the function or handler is known as the epilogue.




