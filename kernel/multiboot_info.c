#include <bolgenos-ng/multiboot_info.h>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/mem_utils.h>

enum mboot_info_flag {
	mbi_mem_info			= (1 << 0)
	/*!<If this flag is set values mem_lower and mem_upper are valid. */,
};


struct __attribute__((packed)) mboot_info {
	uint32_t flags;
	uint32_t mem_lower; /*!< Amount of low memory in kilobytes. */
	uint32_t mem_upper; /*!< Amount of high memory in kilobytes. */
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
assert_type_size(struct mboot_info, 88);

/**
* \brief Boot info from bootloader.
*
* Pointer to boot info structure provided by the bootloader. This symbol
* is set by assembler part of bootstrap code.
* \warning Data that is pointer by this symbol should be copied to kernel
*	internal memory before using memory allocation!
*/
struct mboot_info *__boot_loader_boot_info;

/**
* \brief Boot info structure.
*
* This structure is an own instance of boot info provided by bootloader.
* Structure has valid value only after calling extract_boot_info.
*/
static struct mboot_info boot_info;


void multiboot_info_init() {
	memcpy(&boot_info, __boot_loader_boot_info, sizeof(struct mboot_info));
}

int mboot_is_meminfo_valid() {
	return !! boot_info.flags & mbi_mem_info;
}

uint32_t mboot_get_low_mem() {
	return boot_info.mem_lower;
}

uint32_t mboot_get_high_mem() {
	return boot_info.mem_upper;
}
