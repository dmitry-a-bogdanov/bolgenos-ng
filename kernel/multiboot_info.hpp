#pragma once

#include <cstdint>


/// \brief Multiboot
///
/// Namespace aggregates functionality related to interaction with
/// multiboot-compliant bootloader.
namespace bolgenos {


/// \brief Boot information.
///
/// More info about members can be read in the following article:
/// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
class [[gnu::packed]] MultibootInfo {
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
	std::uint32_t low_memory() const;


	/// \brief Get size of high memory.
	///
	/// Get size of high memory (located upper 1M) provided by
	/// multiboot-compliant bootloader.
	///
	/// \return Size of high memory.
	uint32_t high_memory() const;

	// \brief Initialize multiboot information.
	//
	// Copy boot information structure provided by Multiboot-compliant bootloader to
	// internal kernel memory.
	static void init();
	static const MultibootInfo *instance();

protected:
	uint32_t flags_;	///< Multiboot information status.
	uint32_t mem_lower_;	///< Amount of low memory in kilobytes.
	uint32_t mem_upper_;	///< Amount of high memory in kilobytes.
	uint32_t boot_device;

	/// \brief Command line arguments.
	///
	/// The physical address of the command line to be passed to the kernel.
	/// The command line is a normal C-style zero-terminated string. This
	/// member will be set iff bit 2 of flags_ is set.
	uint32_t cmdline_;
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


} // namespace multiboot
