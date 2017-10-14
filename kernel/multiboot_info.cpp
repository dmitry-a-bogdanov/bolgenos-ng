#include "multiboot_info.hpp"

#include <algorithm>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.hpp>


static_assert(sizeof(bolgenos::MultibootInfo) == 88,
	"bolgenos Information header has wrong size");


namespace {


/// \brief bolgenos information flags.
///
/// bolgenos information flags values for \ref bolgenos::boot_info_t::flags_
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
extern "C" bolgenos::MultibootInfo *temp_boot_info;
bolgenos::MultibootInfo *temp_boot_info;


/// \brief Boot info structure.
///
/// This structure is an own instance of boot info provided by bootloader.
/// Structure has valid value only after calling extract_boot_info.
bolgenos::MultibootInfo boot_info_struct;


} // namespace


static const bolgenos::MultibootInfo *boot_info = &boot_info_struct;

void bolgenos::MultibootInfo::init() {
	boot_info_struct = *temp_boot_info;
}

const bolgenos::MultibootInfo* bolgenos::MultibootInfo::instance()
{
	return boot_info;
}


bool bolgenos::MultibootInfo::is_meminfo_valid() const {
	return flags_ & info_flag_t::mem_info;
}


uint32_t bolgenos::MultibootInfo::low_memory() const {
	return mem_lower_;
}


uint32_t bolgenos::MultibootInfo::high_memory() const {
	return mem_upper_;
}
