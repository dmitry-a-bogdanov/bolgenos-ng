#pragma once

namespace pit {

/**
* \brief Initialize PIT and timer subsystem.
*
* The function initializes PIT and timer subsystem and register timer interrupt
* handler.
*
* \warning This function should be calling before enabling interrupts.
*/
void init();

} // napespace pit
