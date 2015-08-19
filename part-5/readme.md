# Part 5 - Graphics (Basic)

Finally, we get on to a tutorial that moves us away from blinking an LED as we explore the VideoCore IV GPU

## Reference Material

We need some reading material for this tutorial - this is how I put the tutorial together, by reading and studying the manuals available for the Videcore, but for this tutorial mainly the information from the Cambridge Raspberry Pi Tutorials. Yes there's a lot of text and more than one manual - but that's the only way you learn!

Some material that's useful (Generally for late night reading!):

- [Raspberry Pi Firmware Mailboxes information](https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes)

This information is less important for now, but worth noting:

- [VideoCoreIv-AG100 3D Architecture Reference](http://www.broadcom.com/docs/support/videocore/VideoCoreIV-AG100-R.pdf)
- [Android Graphics Driver Source Code for VideoCoreIV](http://www.broadcom.com/docs/support/videocore/Brcm_Android_ICS_Graphics_Stack.tar.gz)

## The GPU (Videocore IV)

The GPU and ARM devices can communicate with each other through a mailbox system. However, don't forget that the ARM and GPU also share the memory space, and so although we have to communicate through the mailbox, this is what negotiates settings and a framebuffer. The framebuffer address in memory is then returned from the GPU and we can go ahead and write to the framebuffer to see graphics.

A framebuffer is a term that really refers to a block of video memory. This video memory is used to display pixels on the screen. So we have access to each pixel on the screen and can change it's colour properties. The framebuffer should be at least as big as the screen resolution. The size of the framebuffer memory block is given by:

```c
    framebuffer_bytes = pixels_x * pixels_y * bytes_per_pixel
```

> **NOTE** In this tutorial we'll be using a framebuffer and so won't have any higher-level functions like OpenGL or accelerated graphics. We're going to create a simple software renderer.

The number of bytes per pixel sets the number of colours available. The Raspberry-Pi GPU supports 8-bit, but in this mode the 8-bit value corresponds to a palette entry, and the palette appears to be very limited A palette mode can be really useful as it's fast (minimal amount of memory required for the graphics) and can be really useful for some special effects by simply altering the palette.

16-bit requires two bytes of data per pixel and directly represents the Red, Green and Blue levels of the colour of the pixel, and 32-bit has 4 bytes of data per pixel which has 8-bit per primary colour and an 8-biot alpha channel (transparency).

The GPU is generally quite a closely guarded secret. It's a specialised processor, but also a powerful processor and most people would like to run code on it, just like we're running code on the GPU itself, but alas the GPU information is still under NDA (Non-Disclosure Agreement) terms.

Broadcom however, did release some information and some of the most interesting information is in the [BCM21553 Graphics Driver]
(http://www.broadcom.com/docs/support/videocore/Brcm_Android_ICS_Graphics_Stack.tar.gz). I'll reference what material I've got from there as we go. Unfortunately there is no definitive source of information for the Raspberry Pi GPU, only bits and pieces scattered around the web. The GPU is a Videocore IV. As we're going to use the mailbox communication with the GPU anyway, we can skip a lot of the GPU detail and just concentrate on communicating with the GPU to get a framebuffer to use.

The mailbox interface is our main entry point into the world of graphics. The interface was developed and created by a few guys at broadcom. The mailbox interface is software running on the GPU which receives messages from software running on the arm and returns responses to each message after performing a task. It's implemented in the start.elf file that we require on the SD Card to boot the Raspberry-Pi. You can see their discussion about implementing the mailbox on [Github](https://github.com/raspberrypi/firmware/issues/47)

The mailboxes are defined [on Github](https://github.com/raspberrypi/firmware/wiki/Mailboxes).

The interface we're interested in is the [Property Interface Mailbox](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface).

This mailbox is responsible for negotiating the framebuffer. We need some code to be able to read and write data from the mailbox and we also need to define the data structure defined by the framebuffer mailbox documentation.

## part-5/armc-014

The ARM014 tutorial introduces a few new peices of the puzzle. Firstly, as an aid to debugging now the code is getting more complex it introduces the mini UART which means we can have a basic "console". As we've bothered with the standard c library we can see how to tie the standard library functions like `printf()` to the UART. Secondly, it introduces the mailbox property interface which is a method of the ARM processor talking to the GPU. If we're going to get to the point of generating graphics, we must talk to the GPU!

This code does generate an "animated" display, but as we'll see - it is extremely slow to use un-optimised software rendering on an RPI! If you want, go ahead and compile it now and plug the raspberry pi into a monitor or television with a HDMI interface. It **should** work! As we're now including new hardware into the mix it's possible that your monitor or TV doesn't support the resolution and colour depth that the example is hard coded to use. It's an example that's designed to be simple rather than supporting every HDMI panel out there. Hopefully you'll have luck with it. I'm using an old Hanns-G HUD19 monitor with DVI->HDMI adaptor.

If it works, you'll see an ever-changing display which moves through the colour spectrum, continuously writing to every pixel in the framebuffer! You can see the (rather boring) output on [YouTube](https://youtube.com).

This tutorial shows how a 700MHz (or 900MHz) processor doesn't give you carte-blanche to program in C and end up with an optimised output. In this example there is no vertical sync used, we simply dedicate 100% of the ARM processor time to drawing the rectangle in video memory and then moving it one pixel before drawing it to the video memory again. In this way we can see the raw speed of the processor at work. It's pretty slow isn't it!

We'll optimise in the ARM015 code later on in this tutorial.

## Getting a UART Text Console

Read that title carefully, not getting a graphics Text Console but instead getting some text out of the code to help us debug. As the code becomes more complex we need better ways of debugging. A later tutorial will talk about using JTAG but for now we can have the basic UART based text debugging that gets us out of most holes!

This requires some hardware. Namely a [TTL-232R-3V3](http://cpc.farnell.com/ftdi/ttl-232r-3v3/cable-usb-to-ttl-level-seri-converter/dp/SC10142) or equivalent is required. The mini uart described in the AUX peripheral below is available on the RPI IO expansion headers on pins 8 (GPIO14/TXD) and 10 (GPIO15/RXD)

Connecting the UART to a PC is pretty easy, a quick connection guide is available:

![UART Connection](https://raw.githubusercontent.com/BrianSidebotham/arm-tutorial-rpi/master/part-5/images/ftdi-ttl-232r-3v3-rpi-connection.png)

Also, a quick photo of one connected up:

![UART Connection](https://raw.githubusercontent.com/BrianSidebotham/arm-tutorial-rpi/master/part-5/images/ftdi-ttl-232r-3v3-rpi-connection-photo.jpg)

### AUX peripheral

The AUX peripheral includes a couple of communication interfaces which we can put to use. In this tutorial we will enable the mini UART which has Rx/Tx signals available on the IO header of the raspberry-pi. We will connect that to an FTDI 3.3V USB->UART converter and then we can connect PuTTY to the COM port and see output from the code!

We will do some more work on the c stubs to provide uart support in the write system call which is what the likes of `printf()`, etc. functions use to write to the system.

```c
#include "rpi-aux.h"
#include "rpi-base.h"
#include "rpi-gpio.h"

static aux_t* auxillary = (aux_t*)AUX_BASE;


aux_t* RPI_GetAux( void )
{
    return auxillary;
}

/* Define the system clock frequency in MHz for the baud rate calculation.
   This is clearly defined on the BCM2835 datasheet errata page:
   http://elinux.org/BCM2835_datasheet_errata */
#define SYS_FREQ    250000000

void RPI_AuxMiniUartInit( int baud, int bits )
{
    volatile int i;

    /* As this is a mini uart the configuration is complete! Now just
       enable the uart. Note from the documentation in section 2.1.1 of
       the ARM peripherals manual:

       If the enable bits are clear you will have no access to a
       peripheral. You can not even read or write the registers */
    auxillary->ENABLES = AUX_ENA_MINIUART;

    /* Disable interrupts for now */
    /* auxillary->IRQ &= ~AUX_IRQ_MU; */

    auxillary->MU_IER = 0;

    /* Disable flow control,enable transmitter and receiver! */
    auxillary->MU_CNTL = 0;

    /* Decide between seven or eight-bit mode */
    if( bits == 8 )
        auxillary->MU_LCR = AUX_MULCR_8BIT_MODE;
    else
        auxillary->MU_LCR = 0;

    auxillary->MU_MCR = 0;

    /* Disable all interrupts from MU and clear the fifos */
    auxillary->MU_IER = 0;

    auxillary->MU_IIR = 0xC6;

    /* Transposed calculation from Section 2.2.1 of the ARM peripherals
       manual */
    auxillary->MU_BAUD = ( SYS_FREQ / ( 8 * baud ) ) - 1;

     /* Setup GPIO 14 and 15 as alternative function 5 which is
        UART 1 TXD/RXD. These need to be set before enabling the UART */
    RPI_SetGpioPinFunction( RPI_GPIO14, FS_ALT5 );
    RPI_SetGpioPinFunction( RPI_GPIO15, FS_ALT5 );

    RPI_GetGpio()->GPPUD = 0;
    for( i=0; i<150; i++ ) { }
    RPI_GetGpio()->GPPUDCLK0 = ( 1 << 14 );
    for( i=0; i<150; i++ ) { }
    RPI_GetGpio()->GPPUDCLK0 = 0;

    /* Disable flow control,enable transmitter and receiver! */
    auxillary->MU_CNTL = AUX_MUCNTL_TX_ENABLE;
}


void RPI_AuxMiniUartWrite( char c )
{
    /* Wait until the UART has an empty space in the FIFO */
    while( ( auxillary->MU_LSR & AUX_MULSR_TX_EMPTY ) == 0 ) { }

    /* Write the character to the FIFO for transmission */
    auxillary->MU_IO = c;
}
```

```c
#ifndef RPI_AUX_H
#define RPI_AUX_H

#include "rpi-base.h"

#define AUX_BASE    ( PERIPHERAL_BASE + 0x215000 )

#define AUX_ENA_MINIUART            ( 1 << 0 )
#define AUX_ENA_SPI1                ( 1 << 1 )
#define AUX_ENA_SPI2                ( 1 << 2 )

#define AUX_IRQ_SPI2                ( 1 << 2 )
#define AUX_IRQ_SPI1                ( 1 << 1 )
#define AUX_IRQ_MU                  ( 1 << 0 )

#define AUX_MULCR_8BIT_MODE         ( 3 << 0 )  /* See errata for this value */
#define AUX_MULCR_BREAK             ( 1 << 6 )
#define AUX_MULCR_DLAB_ACCESS       ( 1 << 7 )

#define AUX_MUMCR_RTS               ( 1 << 1 )

#define AUX_MULSR_DATA_READY        ( 1 << 0 )
#define AUX_MULSR_RX_OVERRUN        ( 1 << 1 )
#define AUX_MULSR_TX_EMPTY          ( 1 << 5 )
#define AUX_MULSR_TX_IDLE           ( 1 << 6 )

#define AUX_MUMSR_CTS               ( 1 << 5 )

#define AUX_MUCNTL_RX_ENABLE        ( 1 << 0 )
#define AUX_MUCNTL_TX_ENABLE        ( 1 << 1 )
#define AUX_MUCNTL_RTS_FLOW         ( 1 << 2 )
#define AUX_MUCNTL_CTS_FLOW         ( 1 << 3 )
#define AUX_MUCNTL_RTS_FIFO         ( 3 << 4 )
#define AUX_MUCNTL_RTS_ASSERT       ( 1 << 6 )
#define AUX_MUCNTL_CTS_ASSERT       ( 1 << 7 )

#define AUX_MUSTAT_SYMBOL_AV        ( 1 << 0 )
#define AUX_MUSTAT_SPACE_AV         ( 1 << 1 )
#define AUX_MUSTAT_RX_IDLE          ( 1 << 2 )
#define AUX_MUSTAT_TX_IDLE          ( 1 << 3 )
#define AUX_MUSTAT_RX_OVERRUN       ( 1 << 4 )
#define AUX_MUSTAT_TX_FIFO_FULL     ( 1 << 5 )
#define AUX_MUSTAT_RTS              ( 1 << 6 )
#define AUX_MUSTAT_CTS              ( 1 << 7 )
#define AUX_MUSTAT_TX_EMPTY         ( 1 << 8 )
#define AUX_MUSTAT_TX_DONE          ( 1 << 9 )
#define AUX_MUSTAT_RX_FIFO_LEVEL    ( 7 << 16 )
#define AUX_MUSTAT_TX_FIFO_LEVEL    ( 7 << 24 )


#define FSEL0(x)        ( x )
...
#define FSEL53(x)       ( x << 9 )


typedef struct {
    volatile unsigned int IRQ;
    volatile unsigned int ENABLES;

    volatile unsigned int reserved1[((0x40 - 0x04) / 4) - 1];

    volatile unsigned int MU_IO;
    volatile unsigned int MU_IER;
    volatile unsigned int MU_IIR;
    volatile unsigned int MU_LCR;
    volatile unsigned int MU_MCR;
    volatile unsigned int MU_LSR;
    volatile unsigned int MU_MSR;
    volatile unsigned int MU_SCRATCH;
    volatile unsigned int MU_CNTL;
    volatile unsigned int MU_STAT;
    volatile unsigned int MU_BAUD;

    volatile unsigned int reserved2[(0x80 - 0x68) / 4];

    volatile unsigned int SPI0_CNTL0;
    volatile unsigned int SPI0_CNTL1;
    volatile unsigned int SPI0_STAT;
    volatile unsigned int SPI0_IO;
    volatile unsigned int SPI0_PEEK;

    volatile unsigned int reserved3[(0xC0 - 0x94) / 4];

    volatile unsigned int SPI1_CNTL0;
    volatile unsigned int SPI1_CNTL1;
    volatile unsigned int SPI1_STAT;
    volatile unsigned int SPI1_IO;
    volatile unsigned int SPI1_PEEK;
    } aux_t;

extern aux_t* RPI_GetAux( void );
extern void RPI_AuxMiniUartInit( int baud, int bits );
extern void RPI_AuxMiniUartWrite( char c );

#endif
```

I'm not going to go too far into explaining these drivers now. It's written in C, and you've got the BCM2835 data sheet the same as I have. You should be getting familiar with the layout of these "driver" files and the documentation in the BCM2835 peripherals document.

We provide an initialisation function so we can set the number of bits (data bits) and the baud rate. The mini UART implementation isn't that configurable because it's not a full UART implementation. It's designed to provide a quick means of providing a console with as little code as possible. So there's no setting for parity or number of stop bits. It's always N1 (No parity, one stop bit).

> **HACK:** The calculation for the baud rate registers is done based on a defined system frequency of 250MHz. It works, but it's not as nice as using a detected system frequency. Perhaps after this tutorial you could get the system frequency and update the calculation to use it!

The functions are written in a blocking mode, so the write function blocks until the UART can accept the next character, then it writes the new character and returns. Normally we'd use the UART interrupt to send a whole buffer of data rather than manually polling the register as this ties the processor up waiting. However, these are easy to use!

### The _write() System Call

Whenever data needs to be written to the OS it's done so through a function called `_write`. This is one of the original c-stubs we wrote in a previous tutorial. We previously just implemented a blank function. Here's the blank function we had. As you can see, it's not quite blank, but `outbyte` does nothing with the data, it's an empty sink.

```c
void outbyte( char b )
{
    (void)b;
}

int _write( int file, char *ptr, int len )
{
    int todo;

    for( todo = 0; todo < len; todo++ )
      outbyte(*ptr++);

    return len;
}
```

We can tie all writes to the OS (for all files) to the UART console by adding the UART write function call to outbyte:

```c
void outbyte( char b )
{
    RPI_AuxMiniUartWrite( b );
}
```

Originally this is what I did, and then when I used printf() to test function I kept getting a crash. In fact after some debugging with the OK LED in the processor exception handlers we introduced in the last tutorial I was able to determine it was an undefined instruction error.

When you run this tutorials examples with PuTTY, you'll see output similar to this:

![PuTTY RPi Connection](https://raw.githubusercontent.com/BrianSidebotham/arm-tutorial-rpi/master/part-5/images/putty-rpi-connection.png)

### Enabling VFP Support

After a bit more thinking and tinkering I realised we have to enable the VFP now in order to use printf. As we're compiling and telling the compiler (or more the linker actually) that we're targeting a device with the VFP it is linking to a libc that can use VFP instructions and why shouldn't something in the printf implementation use the VFP instruction when necessary to speed things up?

There is some [more information about the problem on a stackoverflow answer](http://stackoverflow.com/questions/24589235/application-hangs-when-calling-printf-to-uart-with-bare-metal-raspberry-pi/27257841#27257841).

Without the VFP co-processor being enabled, VFP instructions will cause an undefined instruction exception. Enabling the VFP is another task that the C Runtime startup file crt0 would have performed for us, but we're on our own since we have to use `-nostartfiles` (See an earlier tutorial).

Some [recommended information about enabling VFP support](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0098a/index.html) is available from ARM.

Some [example code for enabling the VFP](http://processors.wiki.ti.com/index.php/Cortex-A8#How_to_enable_NEON) is available on the TI wiki, and that's the code we slice in to armc-start.S so that the VFP is enabled before the c library is setup:

```
    // Enable VFP ------------------------------------------------------------

    // r1 = Access Control Register
    MRC p15, #0, r1, c1, c0, #2
    // enable full access for p10,11
    ORR r1, r1, #(0xf << 20)
    // ccess Control Register = r1
    MCR p15, #0, r1, c1, c0, #2
    MOV r1, #0
    // flush prefetch buffer because of FMXR below
    MCR p15, #0, r1, c7, c5, #4
    // and CP 10 & 11 were only just enabled
    // Enable VFP itself
    MOV r0,#0x40000000
    // FPEXC = r0
    FMXR FPEXC, r0
```

With those additions, we can now go ahead and use the c-library write functions. It means that the full `printf()` implementation can be used on the UART for example without us having to lift a finger and try and implement something as fundamental as that ourselves.

Also, this provides us with a debug/comms channel that is separate to the display. It's hard to use the display for debugging when the display isn't yet working!

## Mailboxes

We introduce some code in a few files, namely `rpi-mailbox` and `rpi-mailbox-interface`. The first is a common interface to access the mailbox system which passes information from the GPU to the ARM processor. The mailbox interface is implemented in the firmware (start.elf) that runs on the GPU.

Here's the code, and as noted in the comments, the mailbox interface is described on the RPI firmware github wiki:

- https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes
- https://github.com/raspberrypi/firmware/wiki/Mailboxes

```c
#ifndef RPI_MAILBOX_H
#define RPI_MAILBOX_H

#include "rpi-base.h"

#define RPI_MAILBOX0_BASE    ( PERIPHERAL_BASE + 0xB880 )

/* The available mailbox channels in the BCM2835 Mailbox interface.
   See https://github.com/raspberrypi/firmware/wiki/Mailboxes for
   information */
typedef enum {
    MB0_POWER_MANAGEMENT = 0,
    MB0_FRAMEBUFFER,
    MB0_VIRTUAL_UART,
    MB0_VCHIQ,
    MB0_LEDS,
    MB0_BUTTONS,
    MB0_TOUCHSCREEN,
    MB0_UNUSED,
    MB0_TAGS_ARM_TO_VC,
    MB0_TAGS_VC_TO_ARM,
} mailbox0_channel_t;

/* These defines come from the Broadcom Videocode driver source code, see:
   brcm_usrlib/dag/vmcsx/vcinclude/bcm2708_chip/arm_control.h */
enum mailbox_status_reg_bits {
    ARM_MS_FULL  = 0x80000000,
    ARM_MS_EMPTY = 0x40000000,
    ARM_MS_LEVEL = 0x400000FF,
};

/* Define a structure which defines the register access to a mailbox.
   Not all mailboxes support the full register set! */
typedef struct {
    volatile unsigned int Read;
    volatile unsigned int reserved1[((0x90 - 0x80) / 4) - 1];
    volatile unsigned int Poll;
    volatile unsigned int Sender;
    volatile unsigned int Status;
    volatile unsigned int Configuration;
    volatile unsigned int Write;
    } mailbox_t;

extern void RPI_Mailbox0Write( mailbox0_channel_t channel, int value );
extern int RPI_Mailbox0Read( mailbox0_channel_t channel );

#endif
```

```c
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
```

There are two mailboxes, 0 and 1. We only need to use mailbox 0. Mailbox has a number of channels to communicate on which are defined in the `enum mailbox0_channel_t`.

The basics of the mailbox operation are pretty straight forward.

To **write** to a mailbox we first construct a composite value. We can write a 32-bit value to the GPU which is generally a memory address, but the mailbox also has to support multiple channels and there is no separate place to write the channel number. In order to achieve multiple channels on a single mailbox the lower nibble (4-bits) are reserved for the channel number and the upper 28-bits are the value we're sending to the GPU. This means that any address we send to the GPU is missing the lowest 4-bits and the GPU simply assumes those bits are all 0. We therefore need to align any address we send the GPU to a 16-byte boundary which will ensure the lower 4-bits will be 0.

The ARM waits for the mailbox to become empty by polling the status register and then it can write the composite value of the address and channel to the mailbox.

To **read** from a mailbox we wait until the mailbox is full and read the value. Only when the value contains the same channel as we are waiting to communicate with do we proceed and return.

The value is a 16-byte aligned memory address. It's important to know that we don't just send a value and get a value back. We send an address to a memory block that we have constructed specifically formatted messages in to communicate with the GPU. The messages that are passed to the GPU are channel dependant and so aren't defined in this code module. We will do a code module for each channel we use. Actually, we're only going to use one - `MB0_TAGS_ARM_TO_VC` which is used with the mailbox property interface.

> **NOTE:** Although the Framebuffer channel looks like the place to begin, actually we'll ignore that mailbox channel because it's a deprecated channel. It came before the mailbox property interface channel had framebuffer properties added to it.

### Mailbox Property Interface

The [Mailbox Property Interface](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface) specifies the messaging structure that needs to be present in the 16-byte aligned memory region I mentioned earlier. There are a lot of properties, and you need to read that page a couple of times to get a hold of how the data needs to be laid out.

Here's the code we're using to support the Mailbox Property Interface:

```c
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "rpi-mailbox.h"
#include "rpi-mailbox-interface.h"

/* Make sure the property tag buffer is aligned to a 16-byte boundary because
   we only have 28-bits available in the property interface protocol to pass
   the address of the buffer to the VC. */
static int pt[8192] __attribute__((aligned(16)));
static int pt_index = 0;


void RPI_PropertyInit( void )
{
    /* Fill in the size on-the-fly */
    pt[PT_OSIZE] = 12;

    /* Process request (All other values are reserved!) */
    pt[PT_OREQUEST_OR_RESPONSE] = 0;

    /* First available data slot */
    pt_index = 2;

    /* NULL tag to terminate tag list */
    pt[pt_index] = 0;
}

/**
    @brief Add a property tag to the current tag list. Data can be included. All data is uint32_t
    @param tag
*/
void RPI_PropertyAddTag( rpi_mailbox_tag_t tag, ... )
{
    va_list vl;
    va_start( vl, tag );

    pt[pt_index++] = tag;

    switch( tag )
    {
        case TAG_GET_FIRMWARE_VERSION:
        case TAG_GET_BOARD_MODEL:
        case TAG_GET_BOARD_REVISION:
        case TAG_GET_BOARD_MAC_ADDRESS:
        case TAG_GET_BOARD_SERIAL:
        case TAG_GET_ARM_MEMORY:
        case TAG_GET_VC_MEMORY:
        case TAG_GET_DMA_CHANNELS:
            /* Provide an 8-byte buffer for the response */
            pt[pt_index++] = 8;
            pt[pt_index++] = 0; /* Request */
            pt_index += 2;
            break;

        case TAG_GET_CLOCKS:
        case TAG_GET_COMMAND_LINE:
            /* Provide a 256-byte buffer */
            pt[pt_index++] = 256;
            pt[pt_index++] = 0; /* Request */
            pt_index += 256 >> 2;
            break;

        case TAG_ALLOCATE_BUFFER:
            pt[pt_index++] = 8;
            pt[pt_index++] = 0; /* Request */
            pt[pt_index++] = va_arg( vl, int );
            pt_index += 1;
            break;

        case TAG_GET_PHYSICAL_SIZE:
        case TAG_SET_PHYSICAL_SIZE:
        case TAG_TEST_PHYSICAL_SIZE:
        case TAG_GET_VIRTUAL_SIZE:
        case TAG_SET_VIRTUAL_SIZE:
        case TAG_TEST_VIRTUAL_SIZE:
        case TAG_GET_VIRTUAL_OFFSET:
        case TAG_SET_VIRTUAL_OFFSET:
            pt[pt_index++] = 8;
            pt[pt_index++] = 0; /* Request */

            if( ( tag == TAG_SET_PHYSICAL_SIZE ) ||
                ( tag == TAG_SET_VIRTUAL_SIZE ) ||
                ( tag == TAG_SET_VIRTUAL_OFFSET ) ||
                ( tag == TAG_TEST_PHYSICAL_SIZE ) ||
                ( tag == TAG_TEST_VIRTUAL_SIZE ) )
            {
                pt[pt_index++] = va_arg( vl, int ); /* Width */
                pt[pt_index++] = va_arg( vl, int ); /* Height */
            }
            else
            {
                pt_index += 2;
            }
            break;


        case TAG_GET_ALPHA_MODE:
        case TAG_SET_ALPHA_MODE:
        case TAG_GET_DEPTH:
        case TAG_SET_DEPTH:
        case TAG_GET_PIXEL_ORDER:
        case TAG_SET_PIXEL_ORDER:
        case TAG_GET_PITCH:
            pt[pt_index++] = 4;
            pt[pt_index++] = 0; /* Request */

            if( ( tag == TAG_SET_DEPTH ) ||
                ( tag == TAG_SET_PIXEL_ORDER ) ||
                ( tag == TAG_SET_ALPHA_MODE ) )
            {
                /* Colour Depth, bits-per-pixel \ Pixel Order State */
                pt[pt_index++] = va_arg( vl, int );
            }
            else
            {
                pt_index += 1;
            }
            break;

        case TAG_GET_OVERSCAN:
        case TAG_SET_OVERSCAN:
            pt[pt_index++] = 16;
            pt[pt_index++] = 0; /* Request */

            if( ( tag == TAG_SET_OVERSCAN ) )
            {
                pt[pt_index++] = va_arg( vl, int ); /* Top pixels */
                pt[pt_index++] = va_arg( vl, int ); /* Bottom pixels */
                pt[pt_index++] = va_arg( vl, int ); /* Left pixels */
                pt[pt_index++] = va_arg( vl, int ); /* Right pixels */
            }
            else
            {
                pt_index += 4;
            }
            break;

        default:
            /* Unsupported tags, just remove the tag from the list */
            pt_index--;
            break;
    }

    /* Make sure the tags are 0 terminated to end the list and update the buffer size */
    pt[pt_index] = 0;

    va_end( vl );
}


int RPI_PropertyProcess( void )
{
    int result;

#if( PRINT_PROP_DEBUG == 1 )
    printf( "%s Length: %d\r\n", __func__, pt[PT_OSIZE] );
#endif
    /* Fill in the size of the buffer */
    pt[PT_OSIZE] = ( pt_index + 1 ) << 2;
    pt[PT_OREQUEST_OR_RESPONSE] = 0;

#if( PRINT_PROP_DEBUG == 1 )
    for( i = 0; i < (pt[PT_OSIZE] >> 2); i++ )
        printf( "Request: %3d %8.8X\r\n", i, pt[i] );
#endif
    RPI_Mailbox0Write( MB0_TAGS_ARM_TO_VC, (unsigned int)pt );

    result = RPI_Mailbox0Read( MB0_TAGS_ARM_TO_VC );

#if( PRINT_PROP_DEBUG == 1 )
    for( i = 0; i < (pt[PT_OSIZE] >> 2); i++ )
        printf( "Response: %3d %8.8X\r\n", i, pt[i] );
#endif
    return result;
}


rpi_mailbox_property_t* RPI_PropertyGet( rpi_mailbox_tag_t tag )
{
    static rpi_mailbox_property_t property;
    int* tag_buffer = NULL;

    property.tag = tag;

    /* Get the tag from the buffer. Start at the first tag position  */
    int index = 2;

    while( index < ( pt[PT_OSIZE] >> 2 ) )
    {
        /* printf( "Test Tag: [%d] %8.8X\r\n", index, pt[index] ); */
        if( pt[index] == tag )
        {
            tag_buffer = &pt[index];
            break;
        }

        /* Progress to the next tag if we haven't yet discovered the tag */
        index += ( pt[index + 1] >> 2 ) + 3;
    }

    /* Return NULL of the property tag cannot be found in the buffer */
    if( tag_buffer == NULL )
        return NULL;

    /* Return the required data */
    property.byte_length = tag_buffer[T_ORESPONSE] & 0xFFFF;
    memcpy( property.data.buffer_8, &tag_buffer[T_OVALUE], property.byte_length );

    return &property;
}
```

```c
#ifndef RPI_MAILBOX_INTERFACE_H
#define RPI_MAILBOX_INTERFACE_H

/**
    @brief An enum of the RPI->Videocore firmware mailbox property interface
    properties. Further details are available from
    https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
*/
typedef enum {
    /* Videocore */
    TAG_GET_FIRMWARE_VERSION = 0x1,

    /* Hardware */
    TAG_GET_BOARD_MODEL = 0x10001,
    TAG_GET_BOARD_REVISION,
    TAG_GET_BOARD_MAC_ADDRESS,
    TAG_GET_BOARD_SERIAL,
    TAG_GET_ARM_MEMORY,
    TAG_GET_VC_MEMORY,
    TAG_GET_CLOCKS,

    /* Config */
    TAG_GET_COMMAND_LINE = 0x50001,

    /* Shared resource management */
    TAG_GET_DMA_CHANNELS = 0x60001,

    /* Power */
    TAG_GET_POWER_STATE = 0x20001,
    TAG_GET_TIMING,
    TAG_SET_POWER_STATE = 0x28001,

    /* Clocks */
    TAG_GET_CLOCK_STATE = 0x30001,
    TAG_SET_CLOCK_STATE = 0x38001,
    TAG_GET_CLOCK_RATE = 0x30002,
    TAG_SET_CLOCK_RATE = 0x38002,
    TAG_GET_MAX_CLOCK_RATE = 0x30004,
    TAG_GET_MIN_CLOCK_RATE = 0x30007,
    TAG_GET_TURBO = 0x30009,
    TAG_SET_TURBO = 0x38009,

    /* Voltage */
    TAG_GET_VOLTAGE = 0x30003,
    TAG_SET_VOLTAGE = 0x38003,
    TAG_GET_MAX_VOLTAGE = 0x30005,
    TAG_GET_MIN_VOLTAGE = 0x30008,
    TAG_GET_TEMPERATURE = 0x30006,
    TAG_GET_MAX_TEMPERATURE = 0x3000A,
    TAG_ALLOCATE_MEMORY = 0x3000C,
    TAG_LOCK_MEMORY = 0x3000D,
    TAG_UNLOCK_MEMORY = 0x3000E,
    TAG_RELEASE_MEMORY = 0x3000F,
    TAG_EXECUTE_CODE = 0x30010,
    TAG_GET_DISPMANX_MEM_HANDLE = 0x30014,
    TAG_GET_EDID_BLOCK = 0x30020,

    /* Framebuffer */
    TAG_ALLOCATE_BUFFER = 0x40001,
    TAG_RELEASE_BUFFER = 0x48001,
    TAG_BLANK_SCREEN = 0x40002,
    TAG_GET_PHYSICAL_SIZE = 0x40003,
    TAG_TEST_PHYSICAL_SIZE = 0x44003,
    TAG_SET_PHYSICAL_SIZE = 0x48003,
    TAG_GET_VIRTUAL_SIZE = 0x40004,
    TAG_TEST_VIRTUAL_SIZE = 0x44004,
    TAG_SET_VIRTUAL_SIZE = 0x48004,
    TAG_GET_DEPTH = 0x40005,
    TAG_TEST_DEPTH = 0x44005,
    TAG_SET_DEPTH = 0x48005,
    TAG_GET_PIXEL_ORDER = 0x40006,
    TAG_TEST_PIXEL_ORDER = 0x44006,
    TAG_SET_PIXEL_ORDER = 0x48006,
    TAG_GET_ALPHA_MODE = 0x40007,
    TAG_TEST_ALPHA_MODE = 0x44007,
    TAG_SET_ALPHA_MODE = 0x48007,
    TAG_GET_PITCH = 0x40008,
    TAG_GET_VIRTUAL_OFFSET = 0x40009,
    TAG_TEST_VIRTUAL_OFFSET = 0x44009,
    TAG_SET_VIRTUAL_OFFSET = 0x48009,
    TAG_GET_OVERSCAN = 0x4000A,
    TAG_TEST_OVERSCAN = 0x4400A,
    TAG_SET_OVERSCAN = 0x4800A,
    TAG_GET_PALETTE = 0x4000B,
    TAG_TEST_PALETTE = 0x4400B,
    TAG_SET_PALETTE = 0x4800B,
    TAG_SET_CURSOR_INFO = 0x8011,
    TAG_SET_CURSOR_STATE = 0x8010

    } rpi_mailbox_tag_t;


typedef enum {
    TAG_STATE_REQUEST = 0,
    TAG_STATE_RESPONSE = 1,
    } rpi_tag_state_t;


typedef enum {
    PT_OSIZE = 0,
    PT_OREQUEST_OR_RESPONSE = 1,
    } rpi_tag_buffer_offset_t;

typedef enum {
    T_OIDENT = 0,
    T_OVALUE_SIZE = 1,
    T_ORESPONSE = 2,
    T_OVALUE = 3,
    } rpi_tag_offset_t;

typedef struct {
    int tag;
    int byte_length;
    union {
        int value_32;
        unsigned char buffer_8[256];
        int buffer_32[64];
    } data;
    } rpi_mailbox_property_t;


extern void RPI_PropertyInit( void );
extern void RPI_PropertyAddTag( rpi_mailbox_tag_t tag, ... );
extern int RPI_PropertyProcess( void );
extern rpi_mailbox_property_t* RPI_PropertyGet( rpi_mailbox_tag_t tag );

#endif
```

> **NOTE:** As you can see, I've left some debugging `printf()` calls in so you can alter `PRINT_PROP_DEBUG` to print them out - this means it'll print out the buffer you message the GPU with and see the resulting buffer when the GPU has finished it's response

We start off with a memory buffer which is large enough to hold a concatenated list of property tags and data for the property tags we want to use. We use a gcc extension to align it to a 16-byte boundary:

```c
static int pt[8192] __attribute__((aligned(16)));
```

The rest of the functions form a C API to the Mailbox Property Interface. There are various APIs people come up with to generate the memory structure.

The messages are constructed such that the data length is large enough for whichever is larger, the message request data size or the response data size. It's also good to note that so long as the data size is large enough the GPU will be happy. i.e. you can use a larger data size for the tag and the GPU won't complain. As you can see from the `void RPI_PropertyAddTag( rpi_mailbox_tag_t tag, ... )` switch `tag` statement there are only a few types of tag layout anyway.

The GPU acts upon tag values which have the request/response code set to process request settings (always 0). When the GPU has parsed a value and filled the tag's value buffer with the value required it sets the request/response indicator to 1 to show that the data in the value buffer is the GPU's response. All of this modifies the data in-place.

The API I wrote for this tutorial allows us to have a simple paradime for using the property interface.

```c
    /* Initialise, Add and Process tags */
    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_*, ... );
    RPI_PropertyAddTag( TAG_*, ... );
    RPI_PropertyAddTag( TAG_*, ... );
    RPI_PropertyProcess();

    /* Get the value for each tag */
    property_value_buffer = RPI_PropertyGet( TAG_* );
```

## The Framebuffer Properties

If you don't yet know, a framebuffer is a block of memory who's data is written to a display. The data organisation depends on the display's attributes such as width, height, colour depth, etc. There is not usually a conversion between one type of data and the framebuffer. The GPU simply clocks the data in the framebuffer to the display.

If a monitor is plugged in to the Raspberry-Pi the GPU detects it and displays a colour gradiant square on the screen. This shows that the RPI is up and running and has detected the screen.

Through the mailbox property interface we can negotiate with the GPU so that it creates a framebuffer in memory that is of the correct size to represent the screen attached (or the size of the virtual screen we've requested). It will return a pointer to that memory so that the ARM has memory it can write to that will be directly written to the screen by the GPU. It's the most basic and simplest form of graphics. Each pixel's colour can be controlled by writing data into the buffer.

Let's look at how we use the property interface to negotiate a 32-bit framebuffer at 1280x1024:

```c
    /* Initialise a framebuffer... */
    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_ALLOCATE_BUFFER );
    RPI_PropertyAddTag( TAG_SET_PHYSICAL_SIZE, 1280, 1024 );
    RPI_PropertyAddTag( TAG_SET_VIRTUAL_SIZE, 1280, 2048 );
    RPI_PropertyAddTag( TAG_SET_DEPTH, 32 );
    RPI_PropertyAddTag( TAG_GET_PITCH );
    RPI_PropertyAddTag( TAG_GET_PHYSICAL_SIZE );
    RPI_PropertyAddTag( TAG_GET_DEPTH );
    RPI_PropertyProcess();
```

As you can see, we set up a load of tags for the GPU to process. We ask it to allocate a framebuffer, so set the physical and virtual size of the screen, and the colour depth. We then also ask it to return us some information, the current pitch, the physical size and the colour depth.

The mailbox interface guarantees that the SET_* tags will be completed before the GET_* tags are processed so we can do a single process with all of the tags in place. Some of the SET_* tags may not be able to achieve what we ask for and so we must use the GET_* tags to know what the framebuffer settings actually are.

The reason there is a physical and virtual size (and why they are set differently!) is because the virtual size can be larger than the physical size where the physical is "mapped" to a part of the framebuffer. The framebuffer is made to hold the largest of the two (which is invariably the virtual size).

In the code I've made the virtual size twice the height of the physical size. There are other mailbox properties that allow us to set an offset where the physical screen will begin in the framebuffer. You can think of the virtual as being the framebuffer and the physical as the screen. We can draw to a larger framebuffer in a region that's off the physical screen and then offset the physical screen to the area of the framebuffer we've drawn to and the update to the screen is instant rather than updating as we draw which usually shows artifacts.

For now, we just initialise the framebuffer (using the code above) and then write to the framebuffer space within the limits of the physical size. We draw a colour gradient box like the GPU does on power-on, but we "animate" it by continuously altering the colour vectors and re-drawing the screen. We use 100% of the CPU processing time to draw and you'll see just how slow this is!

Go ahead and run the example. You can spend some time changing the colour depth setting and screen size to see the performance of the framebuffer fill. You'll notice it's a lot slower than you'd realise to refresh the screen when every pixel is written to.

This is the code that does the actual draw to the framebuffer:

```c
    pixel_offset = ( x * ( bpp >> 3 ) ) + ( y * pitch );

    r = (int)( current_colour.r * 0xFF ) & 0xFF;
    g = (int)( current_colour.g * 0xFF ) & 0xFF;
    b = (int)( current_colour.b * 0xFF ) & 0xFF;
    a = (int)( current_colour.b * 0xFF ) & 0xFF;

    if( bpp == 32 )
    {
        /* Four bytes to write */
        fb[ pixel_offset++ ] = r;
        fb[ pixel_offset++ ] = g;
        fb[ pixel_offset++ ] = b;
        fb[ pixel_offset++ ] = a;
    }
    else if( bpp == 24 )
    {
        /* Three bytes to write */
        fb[ pixel_offset++ ] = r;
        fb[ pixel_offset++ ] = g;
        fb[ pixel_offset++ ] = b;
    }
    else if( bpp == 16 )
    {
        /* Two bytes to write */
        /* Bit pack RGB565 into the 16-bit pixel offset */
        *(unsigned short*)&fb[pixel_offset] = ( (r >> 3) << 11 ) | ( ( g >> 2 ) << 5 ) | ( b >> 3 );
    }
    else
    {
        /* Palette mode. TODO: Work out a colour scheme for
        packing rgb into an 8-bit palette! */
    }
```

As you can see, for each colour depth there is a different drawing algorithm. The RPI also supports an 8-bit palette mode which is not supported here.

The pixel offset calculation can be done based on the bpp (bits-per-pixel) or colour depth setting. Notice that normally we'd use the screen width in this calculation, but instead we're using `pitch`. This is because the GPU can optimse the number of bytes per pixel line so that it can do faster maths, so this is one of the values we request back from the GPU when we initialise the framebuffer.

The framebuffer memory is organised with the top left pixel being at offset 0, in screen coordinates this is 0,0.

> **NOTE:** To notice any animation of the colour with this example, you may have to run the sample code for a little while.

### armc-014

This example expands on the previous pt4 interrupts code and introduces the mailbox API to negotiate a framebuffer with the GPU. It then draws an ever-changing colour square on the screen which changes colour to animate the display. The Frames Per Second (FPS) is also calculated and sent to the mini UART so if you're monitoring the UART with a terminal such as PuTTY you'll be able to see the FPS calculated live on your Pi.

While we acheive that, this does demonstrate how slow software rendering is. The framebuffer is set to 640x480 so that the demo will work on every HDMI panel that's plugged in. 640x480 is not exactly a big screen and yet below are the Frames Per Second (FPS) we managed to achieve on the RPI1 and RPI2.

RPI1 | RPI2
--- | ---
3.67 | 0.68

Those results are the right way round, I promise! It's time for us to do some thinking and optimising...

### armc-015

In the RPI processor (both RPI1 and RPI2) there is a cache system which is disabled by default and is designed to speed the processor up by enabling code to be (hopefully) run from the cache. In this example we add enabling the cache to the `armc-start.S` startup file:

#### Enabling Cache

```
    .equ    SCTLR_ENABLE_DATA_CACHE         0x4
    .equ    SCTLR_ENABLE_BRANCH_PREDICTION  0x800
    .equ    SCTLR_ENABLE_INSTRUCTION_CACHE  0x1000

    // Enable L1 Cache -------------------------------------------------------

    // R0 = System Control Register
    mrc p15,0,r0,c1,c0,0

    // Enable caches and branch prediction
    orr r0,#SCTLR_ENABLE_BRANCH_PREDICTION
    orr r0,#SCTLR_ENABLE_DATA_CACHE
    orr r0,#SCTLR_ENABLE_INSTRUCTION_CACHE

    // System Control Register = R0
    mcr p15,0,r0,c1,c0,0
```

Look to the [ARMv7-a architecture manual](https://silver.arm.com/download/ARM_and_AMBA_Architecture/AR570-DA-70000-r0p0-00rel2/DDI0406C_C_arm_architecture_reference_manual.pdf) for information on enabling L1 Cache. L1 cache is closest to the processor and so is what we're interesting in enabling first off.

Let's go through the code we've added so we know **why** we've added it and also why it works.

**Section B6.1.86** SCTLR, System Control Register, PMSA describes the system control register

This register has some bits defined which are useful for us, namely:

```
    I, bit[12] Instruction cache enable bit.

        This is a global enable bit for instruction caches. The possible values
        of this bit are:

        0 Instruction caches disabled.
        1 Instruction caches enabled.

        If the system does not implement any instruction caches that can be
        accessed by the processor, at any level of the memory hierarchy, this
        bit is RAZ/WI.

        If the system implements any instruction caches that can be accessed
        by the processor then it must be possible to disable them by setting
        this bit to 0.

        Cache enabling and disabling on page B2-1270 describes the effect of
        enabling the caches.

    Z, bit[11] Branch prediction enable bit.

        The possible values of this bit are:

        0 Program flow prediction disabled.
        1 Program flow prediction enabled.

        Setting this bit to 1 enables branch prediction, also called program
        flow prediction.

        If program flow prediction cannot be disabled, this bit is RAO/WI.
        If the implementation does not support program flow prediction then
        this bit is RAZ/WI.

    C, bit[2] Cache enable bit.

        This is a global enable bit for data and unified caches. The possible
        values of this bit are:

        0 Data and unified caches disabled.
        1 Data and unified caches enabled.

        If the system does not implement any data or unified caches that can
        be accessed by the processor, at any level of the memory hierarchy,
        this bit is RAZ/WI.

        If the system implements any data or unified caches that can be
        accessed by the processor then it must be possible to disable them by
        setting this bit to 0.

    For more information about the effect of this bit see Cache enabling and
    disabling on page B2-1270.
```

In the [ARMv6 Architecture Manual](https://silver.arm.com/download/ARM_Architecture/AR550-DA-70002-r0p0-00rel0/DDI%2001001.pdf) for the Pi1 we see:

**Section B3.4** Register 1:Control registers which also descibes the system control register

This control register implements bits in the register we're interested in for enabling L1 cache:

```
    I (bit[12])

        If separate L1 caches are used, this is the enable/disable bit for the L1
        instruction cache:

        0 = L1 instruction cache disabled
        1 = L1 instruction cache enabled.

        If an L1 unified cache is used or the L1 instruction cache is not
        implemented, this bit read as 0 and ignores writes. If the L1 instruction
        cache cannot be disabled, this bit reads as 1 and ignores writes.

        The state of this bit does not affect further levels of cache in the
        system.


    Z (bit[11])

        On ARM processors which support branch prediction, this is the
        enable/disable bit for branch prediction:

        0 = Program flow prediction disabled
        1 = Program flow prediction enabled.

        If program flow prediction cannot be disabled, this bit reads as 1 and
        ignores writes.

        Program flow prediction includes all possible forms of speculative change
        of instruction stream prediction. Examples include static prediction,
        dynamic prediction, and return stacks.

        On ARM processors that do not support branch prediction, this bit reads as
        0 and ignores writes.


    C (bit[2])

        If a L1 unified cache is used, this is the enable/disable bit for the
        unified cache. If separate L1 caches are used, this is the enable/disable
        bit for the data cache. In either case:

        0 = L1 unified/data cache disabled
        1 = L1 unified/data cache enabled.

        If the L1 cache is not implemented, this bit reads as 0 and ignores
        writes. If the L1 cache cannot be disabled, this bit reads as 1 and
        ignores writes.

        The state of this bit does not affect other levels of cache in the system.
```

As can be seen, although the cache system has changed slightly - it is essentially the same for us to use across both RPi1 and RPi2:

Get the value of the System Control Register in R0

```
    // Enable L1 Cache -------------------------------------------------------

    // R0 = System Control Register
    mrc p15,0,r0,c1,c0,0
```

Enable the three cache bits we just identified in the architecture manuals above:

```
    // Enable caches and branch prediction
    orr r0,#SCTLR_ENABLE_BRANCH_PREDICTION
    orr r0,#SCTLR_ENABLE_DATA_CACHE
    orr r0,#SCTLR_ENABLE_INSTRUCTION_CACHE
```

Write the modified value back to the System Control Register:

```
    // System Control Register = R0
    mcr p15,0,r0,c1,c0,0
```

#### Maxmising the Clock Speed (RPI2)

I also added in some more mailbox properties to make the RPI2 run faster. When the RPI2 starts executing code from the ARM the ARM is running at 600MHz which is a way below its 900MHz maximum. Using the mailbox properties interface we can both ask the GPU for the ARMs maximum frequency and then set the ARM frequency to the maximum returned by the GPU:

```c
    mp = RPI_PropertyGet( TAG_GET_MAX_CLOCK_RATE );

    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_SET_CLOCK_RATE, TAG_CLOCK_ARM, mp->data.buffer_32[1] );
    RPI_PropertyProcess();
```

Both the above are fairly easy pickings for making sure we're getting the best speed available out of the processors, yet it was still a bit of work to get the gains!

Run **armc-015** and see the gains! Here's the FPS results:

RPI1 | RPI2
--- | ---
9.8 | 2.9

Wow! A reasonable gain, but still not exactly fast enough to start writing demos or games (Other than text-based games!)

#### RPI2 performance

At this point the question of why the RPI2 is so much slower than the RPI1 should come to mind! I don't have the answer (Send them on a postcard please!)

The only thing going for the RPI1 is a much simpler memory system. Since the RPI2 introduces a quad-core architecture, things got a whole bunch more complicated on the memory bus.

Anyway, let's see what else we can do.

### Making the Compiler work for us

Up until now we've been working with optimisation levels `-O0`(pre-interrupts) and `-01`(post-interrupts). The reason for the move to `-01` was to get rid of the caveat that these tutorials required an earlier version of gcc than the latest that gcc-arm-emedded offered. The issue was down to [this gcc bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59602) and as that bug has sat there for at least one minor version, it was time to fix the issue in the code as opposed to waiting for gcc and gcc-arm-embedded to catch up!

### armc-016

One simple change to this code to see what gains can be had by the compiler. If we're using a C compiler, we really should take advantage of the fact that a massive part of it's job is to optimise the code it generates to run as fast as possible.

We'll change the optimisation level, from `-O1` to `-O4`. Nothing else, let's see what happens to the FPS:

RPI1 | RPI2
--- | ---
11.2 | 62

That is 62, not 6.2 in the RPI2. Clearly the compiler has done something essential for the RPI2, and about what I'd expect for the RPI1 target.

What's it done? I don't know, that's for the compiler to know and for me to find out if I can be bothered. Right now I need get on with writing code.

If you're interested in finding out, you can disassemble the code and see if you can see the major difference between an `-O1` binary and an `-O4` binary!

I've run out of time, so we'll leave graphics here for now and come back to it in a few tutorials time to do some more advanced stuff. We'll look at using hardware accelerated graphics next time.

For now, I think we need to look at JTAG next - I for one am wearing out the SD Card slots on my RPIs testing and re-testing these tutorials!!
