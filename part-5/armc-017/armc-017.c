/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2020, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gic-400.h"

#include "rpi-aux.h"
#include "rpi-armtimer.h"
#include "rpi-framebuffer.h"
#include "rpi-gpio.h"
#include "rpi-interrupts.h"
#include "rpi-mailbox-interface.h"
#include "rpi-systimer.h"

#include "effects.h"
#include "fonts/font09.h"
#include "image-font.h"
#include "starfield.h"

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
#define SCREEN_DEPTH    16

extern void _enable_interrupts(void);

/** Main function - we'll never return from here */
void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
    volatile uint32_t* fb = NULL;
    int width = 0, height = 0;
    int pitch_bytes = 0;
    int pixel_offset;
    unsigned int frame_count = 0;
    rpi_mailbox_property_t *mp;
    uint32_t pixel_value = 0;
    image_t* font_image;
    image_font_t* font;
    rpi_cpu_time_t cputime;

    /* Write 1 to the LED init nibble in the Function Select GPIO peripheral register to enable
       LED pin as an output */
    RPI_SetGpioPinFunction( LED_GPIO, FS_OUTPUT );
    LED_ON();

    /* Using some print statements with no newline causes the output to be buffered and therefore
       output stagnates, so disable buffering on the stdout FILE */
    setbuf(stdout, NULL);

    /* Use the GPU Mailbox to dynamically retrieve the CORE Clock Frequency. This is also what the
       datasheet refers to as the APB (Advanced Peripheral Bus) clock which drives the ARM Timer
       peripheral */
    RPI_PropertyInit();
    RPI_PropertyAddTag(TAG_GET_CLOCK_RATE, TAG_CLOCK_CORE);
    RPI_PropertyProcess();
    mp = RPI_PropertyGet(TAG_GET_CLOCK_RATE);
    uint32_t core_frequency = mp->data.buffer_32[1];

    /* Calculate the timer reload register value so we achieve an interrupt rate of 2Hz. Every
       second interrupt will therefore be one second. It's approximate, the division doesn't
       really work out to be precisely 1s because of the divisor options and the core
       frequency. */
    uint16_t prescales[] = {1, 16, 256, 1};
    uint32_t timer_load = (1.0 / 2) / (1.0/(core_frequency / (RPI_GetArmTimer()->PreDivider + 1) * (prescales[(RPI_GetArmTimer()->Control & 0xC) >> 2])));
    RPI_GetArmTimer()->Load = timer_load;

    /* Setup the ARM Timer */
    RPI_GetArmTimer()->Control = ( RPI_ARMTIMER_CTRL_23BIT |
            RPI_ARMTIMER_CTRL_ENABLE | RPI_ARMTIMER_CTRL_INT_ENABLE );

    /* Enable the ARM Interrupt controller in the BCM interrupt controller */
    RPI_EnableARMTimerInterrupt();

    /* Globally enable interrupts */
    _enable_interrupts();

    /* Initialise the UART */
    RPI_AuxMiniUartInit( 115200, 8 );

    /* Wait 500ms to let UART to settle before we use it. The UART takes a little time to switch
       speeds. Comment out this to see some garbage at the start of the UART output if you like */
    RPI_WaitMicroSeconds( 500000 );

    /* Print to the UART using the standard libc functions */
    printf( "\r\n" );
    printf( "------------------------------------------\r\n" );
    printf( "Valvers.com ARM Bare Metal Tutorials\r\n" );
    printf( "Initialise UART console with standard libc\r\n" );

    printf("CORE Frequency: %dMHz\r\n", (core_frequency / 1000000));

    /* Clock Frequency */
    RPI_PropertyInit();
    RPI_PropertyAddTag(TAG_GET_MAX_CLOCK_RATE, TAG_CLOCK_ARM);
    RPI_PropertyProcess();

    mp = RPI_PropertyGet( TAG_GET_MAX_CLOCK_RATE );

    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_SET_CLOCK_RATE, TAG_CLOCK_ARM, mp->data.buffer_32[1] );
    RPI_PropertyProcess();

    RPI_PropertyInit();
    RPI_PropertyAddTag(TAG_GET_CLOCK_RATE, TAG_CLOCK_ARM);
    RPI_PropertyProcess();

    if( mp = RPI_PropertyGet(TAG_GET_CLOCK_RATE) ) {
        printf("ARM  Frequency: %dMHz\r\n", (mp->data.buffer_32[1] / 1000000));
    }

    RPI_PropertyInit();
    RPI_PropertyAddTag( TAG_GET_BOARD_REVISION );
    RPI_PropertyAddTag( TAG_GET_FIRMWARE_VERSION );
    RPI_PropertyAddTag( TAG_GET_BOARD_MAC_ADDRESS );
    RPI_PropertyAddTag( TAG_GET_BOARD_SERIAL );
    RPI_PropertyProcess();

    const char* processors[] = { "BCM2835", "BCM2836", "BCM2837", "BCM2711" };

    const char* rpi_types[] = {
        "1A", "1B", "1A+", "1B+", "2B", "ALPHA", "CM1", "{7}", "3B", "Zero", "CM3", "{11}", "Zero W", "3B+",
        "3A+", "-", "CM3+", "4B" };

    const char* rpi_memories[] = {
        "256MB", "512MB", "1GiB", "2GiB", "4GiB", "8GiB" };

    const char* rpi_manufacturers[] = {
        "Sony UK", "Egoman", "Embest", "Sony Japan", "Embest", "Stadium" };

    const char* rpi_models[] = {
        "-", "-",
        "RPI1B 1.0 256MB Egoman",
        "RPI1B 1.0 256MB Egoman",
        "RPI1B 2.0 256MB Sony UK",
        "RPI1B 2.0 256MB Qisda",
        "RPI1B 2.0 256MB Egoman",
        "RPI1A 2.0 256MB Egoman",
        "RPI1A 2.0 256MB Sony UK",
        "RPI1A 2.0 256MB Qisda",
        "RPI1B 2.0 512MB Egoman",
        "RPI1B 2.0 512MB Sony UK",
        "RPI1B 2.0 512MB Egoman",
        "RPI1B+ 1.2 512MB Sony UK",
        "CM1 1.0 512MB Sony UK",
        "RPI1A+ 1.1 256MB Sony UK",
        "RPI1B+ 1.2 512MB Embest",
        "CM1 1.0 512MB Embest",
        "RPI1A+ 1.1 256MB/512MB Embest",
    };

    if( mp = RPI_PropertyGet( TAG_GET_BOARD_REVISION ) ) {
        uint32_t revision = mp->data.value_32;
        printf("Board Revision: 0x%8.8x", mp->data.value_32);
        if ( revision & ( 1 << 23 ) ) {
            /* New style revision code */
            printf(" rpi-%s", rpi_types[(revision & (0xFF << 4)) >> 4]);
            printf(" %s", processors[(revision & (0xF << 12)) >> 12]);
            printf(" %s", rpi_memories[(revision & (0x7 << 20)) >> 20]);
            printf(" %s", rpi_manufacturers[(revision & (0xF << 16)) >> 16]);
        } else {
            /* old style revision code */
            printf(" %s", rpi_models[revision]);
        }

        printf("\r\n");
    }

    if( mp = RPI_PropertyGet( TAG_GET_FIRMWARE_VERSION ) ) {
        printf( "Firmware Version: %d\r\n", mp->data.value_32 );
    }

    if( mp = RPI_PropertyGet( TAG_GET_BOARD_MAC_ADDRESS ) ) {
        printf( "MAC Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\r\n",
                mp->data.buffer_8[0], mp->data.buffer_8[1], mp->data.buffer_8[2],
                mp->data.buffer_8[3], mp->data.buffer_8[4], mp->data.buffer_8[5] );
    }

    if( mp = RPI_PropertyGet( TAG_GET_BOARD_SERIAL ) ) {
        printf( "Serial Number: %8.8X%8.8X\r\n", mp->data.buffer_32[0], mp->data.buffer_32[1] );
    }

    RPI_InitFramebuffer( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH );

    font_image = image16_from_gimp( &font09 );
    font = font_from_image( 29, 35, font_image, 0 );

    RPI_GetCurrentCpuTime( &cputime );

    int idx = 0;
    int screen_centre = ( RPI_GetFramebuffer()->physical_height >> 1 ) - ( font->pixel_height >> 1 );

    sinewave_effect_t* text_fx = FX_NewSine((sinewave_settings_t){
            .amplitude = 45,
            .frequency = 1,
            .speed = 4,
            .fb = RPI_GetFramebuffer() });

    sinewave_effect_t* position_fx = FX_NewSine((sinewave_settings_t){
            .amplitude = 100,
            .frequency = 2,
            .speed = 1,
            .fb = RPI_GetFramebuffer() });

    while( 1 )
    {
        /* Force 50Hz Framerate - we do not have access to a vsync :( */
        RPI_ClearScreen();
        process_starfield();
        FX_AnimateSine( text_fx );
        FX_AnimateSine( position_fx );

        font_puts( 200, screen_centre + position_fx->effect.vertical_blit_y_processor(0, &position_fx->effect ),
                   "HELLO WORLD!", font, &text_fx->effect );

        RPI_SwitchFramebuffer();
        RPI_TimeEvent( &cputime, 20000 );

        frame_count++;

        if( uptime && ( ( uptime % 10 ) == 0 ) ) {
            float fps = (float)frame_count / uptime;
            printf( "Uptime: %4ds Frames: %10d FPS: %.2f\r\n", uptime, frame_count, fps );
        }
    }
}
