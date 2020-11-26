/*
    Part of the Raspberry-Pi Bare Metal Tutorials
    https://www.valvers.com/rpi/bare-metal/
    Copyright (c) 2013-2018, Brian Sidebotham

    This software is licensed under the MIT License.
    Please see the LICENSE file included with this software.

*/

#include <stdint.h>
#include "rpi-systimer.h"

static rpi_sys_timer_t* rpiSystemTimer = (rpi_sys_timer_t*)RPI_SYSTIMER_BASE;

rpi_sys_timer_t* RPI_GetSystemTimer(void)
{
    return rpiSystemTimer;
}

/**
* @fn uint32_t RPI_GetCurrentTime( void )
* @brief Returns the current CPU time in microseconds. Limited to 32-bit width and will wrap.
*/
void RPI_GetCurrentCpuTime( rpi_cpu_time_t* cputime )
{
    cputime->hi = rpiSystemTimer->counter_hi;
    cputime->lo = rpiSystemTimer->counter_lo;

    /* Allow for the non-atomic operation where hi could have changed as lo overflowed during the
       two instructions above. If it does happen we can detect it and get a new snapshot of the cpu
       time knowing that it will not have overflowed in the time taken to execute the next set of
       instructions */
    if( cputime->hi != rpiSystemTimer->counter_hi )
    {
        cputime->hi = rpiSystemTimer->counter_hi;
        cputime->lo = rpiSystemTimer->counter_lo;
    }
}

/**
* @fn void RPI_TimeEvent( uint32_t us )
* @brief Returns after us since the last call of this function (Single threaded!)
*/
void RPI_TimeEvent( rpi_cpu_time_t* cputime, uint32_t us )
{
    rpi_cpu_time_t current_time;
    uint32_t inbound_time = cputime->lo;
    cputime->lo += us;

    if( inbound_time > cputime->lo )
    {
        /* The lo register overflowed, so increas the high timer */
        cputime->hi += 1;
    }

    while(1)
    {
        RPI_GetCurrentCpuTime( &current_time );
        if( ( current_time.hi > cputime->hi ) ||
          ( ( current_time.hi == cputime->hi ) && ( current_time.lo >= cputime->lo ) ) )
        {
            break;
        }
    }
}

void RPI_WaitMicroSeconds( uint32_t us )
{
    volatile uint32_t ts = rpiSystemTimer->counter_lo;

    while( ( rpiSystemTimer->counter_lo - ts ) < us )
    {
        /* BLANK */
    }
}
