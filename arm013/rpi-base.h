
#ifndef RPI_BASE_H
#define RPI_BASE_H

#include <stdint.h>

#define _set_interrupt_vector_base( x )     asm volatile( "mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" ( x ) )
#define _enable_interrupts()                asm volatile( "cpsie i" )

typedef volatile uint32_t rpi_reg_rw_t;
typedef volatile const uint32_t rpi_reg_ro_t;
typedef volatile uint32_t rpi_reg_wo_t;

typedef volatile uint64_t rpi_wreg_rw_t;
typedef volatile const uint64_t rpi_wreg_ro_t;

#endif
