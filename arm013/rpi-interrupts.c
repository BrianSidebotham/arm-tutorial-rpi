
#include <stdint.h>

/* A struct that includes our basic interrupt vectors */
#include "rpi-interrupts.h"

extern uint32_t _exception_table;

rpi_irq_controller_t* rpiIRQController = (rpi_irq_controller_t*)RPI_INTERRUPT_CONTROLLER_BASE;

void reset_vector(void) __attribute__((interrupt("ABORT")));
void undefined_instruction_vector(void) __attribute__((interrupt("UNDEF")));
void supervisor_call_vector(void) __attribute__((interrupt("SWI")));
void prefetch_abort_vector(void) __attribute__((interrupt("ABORT")));
void data_abort_vector(void) __attribute__((interrupt("ABORT")));
void interrupt_vector(void) __attribute__((interrupt("IRQ")));
void fast_interrupt_vector(void) __attribute__((interrupt("FIQ")));


void reset_vector(void) { }
void undefined_instruction_vector(void) { }
void supervisor_call_vector(void) { }
void prefetch_abort_vector(void) { }
void data_abort_vector(void) { }
void interrupt_vector(void) { }
void fast_interrupt_vector(void) { }

void pre_main_init_exceptions(void)
{
    uint32_t* tp = &_exception_table;
    uint32_t* exceptions = (void*)0;
    int i;

    /* Copy all 7 exception vectors, AND the UNUSED vector! */
    for( i=0; i<8; i++ )
        exceptions[i] = tp[i];
}
