#ifndef __BOLGENOS_NG__TIME_H__
#define __BOLGENOS_NG__TIME_H__

#include <bolgenos-ng/int_types.h>


/**
* \brief Current ticks number.
*
* This variable keeps number of ticks since start of kernel.
*/
extern volatile uint32_t ticks;


/**
* \brief Initialize timer subsystem.
*
* The function initializes timer subsystem and register timer interrupt in
*	interrupt handlers table.
* \warning Timer subsystem should be initilized before the first enabling
*	of interrupts.
*/
void init_timer();


/**
* \brief Do nothing during specified time.
*
* Function does nothing during specified at least specified time.
* \param ticks_timeout Timeout for doing nothing in ticks.
*/
void __sleep(uint32_t ticks_timeout);
// TODO: version with timeout in ms is needed!


#endif // __BOLGENOS_NG__TIME_H__
