#pragma once

#include <bolgenos-ng/stdtypes>


/// \brief Multiboot
///
/// Namespace aggregates functionality related to interaction with
/// multiboot-compliant bootloader.
namespace multiboot {


class __attribute__((packed)) boot_info_t {
public:
	/// \brief Check that memory information is valid.
	///
	/// Check that multiboot-compliant bootloader provided valid information
	/// about memory on this machine.
	bool is_meminfo_valid() const;


	/// \brief Get size of low memory.
	///
	/// Get size of low memory (located lower 1M) provided by
	/// multiboot-compliant bootloader.
	///
	/// \return Size of low memory.
	uint32_t low_memory() const;


	/// \brief Get size of high memory.
	///
	/// Get size of high memory (located upper 1M) provided by
	/// multiboot-compliant bootloader.
	///
	/// \return Size of high memory.
	uint32_t high_memory() const;

protected:
	uint32_t flags_;	///< Multiboot information status.
	uint32_t mem_lower_;	///< Amount of low memory in kilobytes.
	uint32_t mem_upper_;	///< Amount of high memory in kilobytes.
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint8_t syms[16];
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};
static_assert(sizeof(boot_info_t) == 88,
	"Multiboot Information header has wrong size");


// \brief Initialize multiboot information.
//
// Copy boot information structure provided by Multiboot-compliant bootloader to
// internal kernel memory.
void init();


/// \brief Boot information
///
/// Pointer to information that is got from multiboot-compliant bootloader.
extern const boot_info_t *boot_info;


} // namespace multiboot
