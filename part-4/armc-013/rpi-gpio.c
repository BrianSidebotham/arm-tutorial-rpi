
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
