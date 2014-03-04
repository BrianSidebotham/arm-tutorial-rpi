
#ifndef RPI_SYSTIMER_H
#define RPI_SYSTIMER_H

#include <stdint.h>

#include "rpi-base.h"

/** @brief See the documentation for the ARM side timer (Section 14 of the
    BCM2835 Peripherals PDF) */
#define RPI_ARMTIMER_BASE       0x2000B400


/** @brief Section 14.2 of the BCM2835 Peripherals documentation details
    the register layout for the ARM side timer */
typedef struct {
    volatile uint32_t Load;
    volatile uint32_t Value;
    volatile uint32_t Control;
    volatile uint32_t IRQClear;
    volatile uint32_t RAWIRQ;
    volatile uint32_t MaskedIRQ;
    volatile uint32_t Reload;
    volatile uint32_t PreDivider;
    volatile uint32_t FreeRunningCounter;
    } rpi_arm_timer_t;


extern rpi_arm_timer_t* RPI_GetArmTimer(void);
extern void RPI_ArmTimerInit(void);

#endif
