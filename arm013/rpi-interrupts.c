
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


void reset_vector(void)
{

}

void undefined_instruction_vector(void)
{

}


void supervisor_call_vector(void)
{

}


void prefetch_abort_vector(void)
{

}


void data_abort_vector(void)
{

}


void interrupt_vector(void)
{

}


void fast_interrupt_vector(void)
{

}


void pre_main_init_exceptions(void)
{
    /* Set the interrupt base register */
    asm volatile( "mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (&_exception_table) );

    /* Enable interrupts! */
    asm volatile( "cpsie i" );

    /* Enable the timer interrupt IRQ */
    rpiIRQController->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

    /* Setup the system timer interrupt */
}
