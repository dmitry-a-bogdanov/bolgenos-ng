#include <bolgenos-ng/kernel_object.hpp>

#include <cstddef>
#include <cstdint>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/page.hpp>
#include <bolgenos_config.hpp>


extern "C"
{

extern memory::page_frame_t pre_ks[config::STACK_CANARY_PAGES];
extern unsigned char ks[config::STACK_SIZE];
extern memory::page_frame_t post_ks[config::STACK_CANARY_PAGES];


}
// extern "C"

memory::page_frame_t pre_ks[config::STACK_CANARY_PAGES]		[[gnu::section(".pre_ks_canary")]];
unsigned char ks[config::STACK_SIZE]						[[gnu::section(".ks")]];
memory::page_frame_t post_ks[config::STACK_CANARY_PAGES]	[[gnu::section(".post_ks_canary")]];



/// \brief Start of kernel object.
///
/// Pointer to start of memory where kernel ELF is loaded.
_asm_linked_ char __object_begin[];


/// \brief End of kernel object.
///
/// Pointer to end of memory where kernel ELF is loaded.
_asm_linked_ char __object_end[];


_asm_linked_ char __text_begin[];
_asm_linked_ char __text_end[];
_asm_linked_ char __stack_begin[];
_asm_linked_ char __stack_end[];


void* kobj::begin() {
	return static_cast<void *>(__object_begin);
}


void* kobj::end() {
	return static_cast<void *>(__object_end);
}


void* kobj::code_begin() {
	return static_cast<void *>(__text_begin);
}


void* kobj::code_end() {
	return static_cast<void *>(__text_end);
}


void* kobj::stack_begin() {
	return static_cast<void *>(__stack_begin);
}

void* kobj::stack_end() {
	return static_cast<void *>(__stack_end);
}

