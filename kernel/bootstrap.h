#ifndef __BOLGENOS_NG__BOOTSTRAP_H__
#define __BOLGENOS_NG__BOOTSTRAP_H__

/**
* \brief Kernel stack size.
*
* Macro defines size of kernel bootstrap stack in bytes.
*/
#if defined(CONFIG_KERNEL_STACK_SIZE)
#	define KERNEL_STACK_SIZE (CONFIG_KERNEL_STACK_SIZE)
#else
#	define KERNEL_STACK_SIZE (1024*16*1024)
#endif


#if !defined(__ASSEMBLER__)

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/mem_utils.h>


#define __MBH_MAGIC (0x1badb002)

#define MBH_ALIGN (1 << 0)
#define MBH_MEMINFO (1 << 1)

#define __mbh_checksum(flags_value) (-(__MBH_MAGIC + (flags_value)))

typedef struct __attribute__((packed)) {
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
	uint32_t mode_type;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
} multiboot_header_t;


#define mbh_initializer(flags_value) {				\
		.magic = __MBH_MAGIC,				\
		.flags = (flags_value),				\
		.checksum = __mbh_checksum(flags_value)		\
	}


/**
* \brief Multiboot info: memory.
*
* If this flag is set values mem_lower and mem_upper are valid.
*/
#define MBI_MEM_INFO			(1 << 0)


typedef struct __attribute__((packed)) {
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
} boot_info_t;
check_type_size(boot_info_t, 88);


#endif

#endif // __BOLGENOS_NG__BOOTSTRAP_H__
