#ifndef __BOLGENOS_NG__BOOTSTRAP_H__
#define __BOLGENOS_NG__BOOTSTRAP_H__ (1)

/*
 * KERNEL_STACK_SIZE defines size of kernel bootstrap stack in bytes
 */
#if defined(CONFIG_KERNEL_STACK_SIZE)
#	define KERNEL_STACK_SIZE (CONFIG_KERNEL_STACK_SIZE)
#else
#	define KERNEL_STACK_SIZE (1024*16*1024)
#endif



#if !defined(__ASSEMBLER__)

#include <bolgenos-ng/int_types.h>

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

#endif

#endif // __BOLGENOS_NG__BOOTSTRAP_H__
