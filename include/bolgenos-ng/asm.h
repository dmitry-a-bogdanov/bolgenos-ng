#ifndef __BOLGENOS_NG__ASM_H__
#define __BOLGENOS_NG__ASM_H__ (1)

#include <bolgenos-ng/int_types.h>

#define __link_from_asm__ extern


typedef struct __attribute__((packed)) {
	uint16_t limit:16;
	uint32_t base:32;
} descriptor_table_ptr_t;

#endif // __BOLGENOS_NG__ASM_H__
