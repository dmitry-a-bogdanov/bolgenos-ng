#pragma once


namespace kobj {


/// \brief Start of kernel object.
///
/// Pointer to start of memory where kernel ELF is loaded.
void* begin();


/// \brief End of kernel object.
///
/// Pointer to end of memory where kernel ELF is loaded.
void* end();


/// Pointer to the beginning of kernel code.
void* code_begin();


/// Pointer to the end of kernel code.
void* code_end();


/// Pointer to the begging of kernel stack.
void* stack_begin();


/// Pointer to the end of kernel stack.
void* stack_end();


} // namespace kernel
