/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#include <stdint.h>
#include "rpi-gpio.h"

static rpi_gpio_t* rpiGpio = (rpi_gpio_t*)RPI_GPIO_BASE;

rpi_gpio_t* RPI_GetGpio(void)
{
    return rpiGpio;
}

void RPI_GpioInit(void)
{

}
