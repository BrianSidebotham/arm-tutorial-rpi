/**
    @file armc-2.c
    @brief ARM C Tutorial #2 for Raspberry-Pi

    Using C without an operating system on the Raspberry-Pi BCM2835 board.

    This example lights the OK LED and then loops infinitely
*/

/** The base address of the GPIO peripheral (ARM Physical Address) */
#define GPIO_BASE       0x20200000UL

#define GPIO_GPFSEL0    0
#define GPIO_GPFSEL1    1
#define GPIO_GPFSEL2    2
#define GPIO_GPFSEL3    3
#define GPIO_GPFSEL4    4
#define GPIO_GPFSEL5    5

#define GPIO_GPSET0     7
#define GPIO_GPSET1     8

#define GPIO_GPCLR0     10
#define GPIO_GPCLR1     11

#define GPIO_GPLEV0     13
#define GPIO_GPLEV1     14

#define GPIO_GPEDS0     16
#define GPIO_GPEDS1     17

#define GPIO_GPREN0     19
#define GPIO_GPREN1     20

#define GPIO_GPFEN0     22
#define GPIO_GPFEN1     23

#define GPIO_GPHEN0     25
#define GPIO_GPHEN1     26

#define GPIO_GPLEN0     28
#define GPIO_GPLEN1     29

#define GPIO_GPAREN0    31
#define GPIO_GPAREN1    32

#define GPIO_GPAFEN0    34
#define GPIO_GPAFEN1    35

#define GPIO_GPPUD      37
#define GPIO_GPPUDCLK0  38
#define GPIO_GPPUDCLK1  39

/** GPIO Register set */
volatile unsigned int* gpio;

/** Main function - we'll never return from here */
int main(void)
{
    /* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
    gpio = (unsigned int*)GPIO_BASE;

    /* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
       peripheral register to enable GPIO16 as an output */
    gpio[GPIO_GPFSEL1] |= (1 << 18);

    /* Write 1 to the 16th bit in the Clear0 GPIO peripheral register to set
       the output level of GPIO16 as 0 (logic low) */
    gpio[GPIO_GPCLR0] = (1 << 16);

    /* Never exit as there is no OS to exit to! */
    while(1)
    {

    }
}
