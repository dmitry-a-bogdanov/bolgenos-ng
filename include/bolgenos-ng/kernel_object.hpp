#pragma once


namespace kobj {



/**
* \brief Start of kernel object.
*
* Pointer to start of memory where kernel ELF is loaded.
*
*/
void * begin();


/**
* \brief End of kernel object.
*
* Pointer to end of memory where kernel ELF is loaded.
*/
void * end();


void * code_begin();
void * code_end();

void * stack_begin();
void * stack_end();


} // namespace kernel
