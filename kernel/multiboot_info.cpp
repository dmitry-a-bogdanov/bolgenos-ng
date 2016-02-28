#include <bolgenos-ng/multiboot_info.hpp>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.h>

namespace {


/// \brief Multiboot information flags.
///
/// Multiboot information flags values for \ref multiboot::boot_info_t::flags_
enum info_flag_t:uint32_t {
	/// If this flag is set values mem_lower and mem_upper are valid.
	mem_info			= (1 << 0),
};


/// \brief Boot info from bootloader.
///
/// Pointer to boot info structure provided by the bootloader. This symbol
/// is set by assembler part of bootstrap code.
/// Also it must be declared as `extern "C"` for linkage with asm code.
/// \warning Data that is pointer by this symbol should be copied to kernel
/// internal memory before using memory allocation!
extern "C" multiboot::boot_info_t *temp_boot_info;
multiboot::boot_info_t *temp_boot_info;


/// \brief Boot info structure.
///
/// This structure is an own instance of boot info provided by bootloader.
/// Structure has valid value only after calling extract_boot_info.
multiboot::boot_info_t boot_info_struct;


} // namespace


const multiboot::boot_info_t *multiboot::boot_info = &boot_info_struct;


void multiboot::init() {
	memcpy(&boot_info_struct, temp_boot_info, sizeof(boot_info_t));
}


bool multiboot::boot_info_t::is_meminfo_valid() const {
	return flags_ & info_flag_t::mem_info;
}


uint32_t multiboot::boot_info_t::low_memory() const {
	return mem_lower_;
}


uint32_t multiboot::boot_info_t::high_memory() const {
	return mem_upper_;
}
