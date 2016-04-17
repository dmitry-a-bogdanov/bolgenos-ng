#include <bolgenos-ng/kernel_object.hpp>

#include <bolgenos-ng/asm.h>

/**
* \brief Start of kernel object.
*
* Pointer to start of memory where kernel ELF is loaded.
*
*/
_asm_linked_ char __object_begin[0];


/**
* \brief End of kernel object.
*
* Pointer to end of memory where kernel ELF is loaded.
*/
_asm_linked_ char __object_end[0];


_asm_linked_ char __text_begin[0];
_asm_linked_ char __text_end[0];
_asm_linked_ char __stack_begin[0];
_asm_linked_ char __stack_end[0];


void * kobj::begin() {
	return static_cast<void *>(__object_begin);
}


void * kobj::end() {
	return static_cast<void *>(__object_end);
}


void * kobj::code_begin() {
	return static_cast<void *>(__text_begin);
}


void * kobj::code_end() {
	return static_cast<void *>(__text_end);
}


void * kobj::stack_begin() {
	return static_cast<void *>(__stack_begin);
}

void * kobj::stack_end() {
	return static_cast<void *>(__stack_end);
}

