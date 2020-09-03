
#include "rpi-base.h"
#include "gic-400.h"

typedef struct {
    gic400_gicd_t* gicd;
    gic400_gicc_t* gicc;
} gic400_t;

static gic400_t gic400;

int gic400_init(void* interrupt_controller_base)
{
    int number_of_interrupts = 0;

    gic400.gicd = (gic400_gicd_t*)(interrupt_controller_base + 0x1000);
    gic400.gicc = (gic400_gicc_t*)(interrupt_controller_base + 0x2000);

    /* Disable the controller so we can configure it before it passes any
       interrupts to the CPU */
    gic400.gicd->ctl = GIC400_CTL_DISABLE;

    /* Get the number of interrupt lines implemented in the GIC400 controller */
    number_of_interrupts = GIC400_TYPE_ITLINESNUMBER_GET(gic400.gicd->type) * 32;

    /* The actual number returned by the ITLINESNUMBER is the number of registered implemented.
       The actual number of interrupt lines available is (ITLINESNUMBER * 32) */
    for( int i = 0; i < number_of_interrupts; i++ ) {
        /* Deal with register sets that have 32-interrupts per register */
        if( ( i % 32 ) == 0 ) {
            /* Disable this block of 32 interrupts, clear the pending and active flags */
            gic400.gicd->icenable[i/32] = 0xFFFFFFFF;
            gic400.gicd->icpend[i/32] = 0xFFFFFFFF;
            gic400.gicd->icactive[i/32] = 0xFFFFFFFF;
        }
        /* Deal with interrupt configuration. The configuration registers are 32-bit wide and
        have 2-bit configuration for each interrupt */
        gic400.gicd->icfg[i/16] &= (0x3 << (i % 16));
        gic400.gicd->icfg[i/16] |= (GIC400_ICFG_LEVEL_SENSITIVE << (i % 16));

        /* Deal with register sets that have one interrupt per register */
        gic400.gicd->ipriority[i] = 0xA0;
        gic400.gicd->istargets[i] = GIC400_TARGET_CPU0;
    }

    gic400.gicd->ctl = GIC400_CTL_ENABLE;

    gic400.gicc->pm = 0xF0;
    gic400.gicc->ctl = GIC400_CTL_ENABLE;
}
