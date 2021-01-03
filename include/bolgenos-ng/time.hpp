#pragma once

#include <cstdint.hpp>

/**
* \brief Current ticks number.
*
* This variable keeps number of ticks since start of kernel.
*/
extern volatile uint32_t jiffies;


/**
* \brief Do nothing during specified time.
*
* Function does nothing during specified at least specified time.
* \param ticks_timeout Timeout for doing nothing in ticks.
*/
void __sleep(uint32_t ticks_timeout);

/**
* \brief Do nothing during specified time.
*
* Function does nothing during specified at least specified time.
* \param ms Timeout for doing nothing in milliseconds.
*/
void sleep_ms(uint32_t ms);


/**
* \brief Convert milliseconds to timer ticks.
*
* Function converts given number of milliseconds to timer ticks with rounding
* to up.
* \param ms Number of milliseconds to be converted.
* \return Number of ticks.
*/
uint32_t ms_to_ticks(uint32_t ms);

