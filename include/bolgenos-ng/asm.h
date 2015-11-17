#ifndef __BOLGENOS_NG__ASM_H__
#define __BOLGENOS_NG__ASM_H__ (1)

#include <bolgenos-ng/int_types.h>

#define __link_from_asm__ extern


typedef struct __attribute__((packed)) {
	uint16_t limit:16;
	uint32_t base:32;
} descriptor_table_ptr_t;


static inline void outb(uint16_t port, uint8_t byte) {
	asm volatile ("outb %0, %1":: "a"(byte), "Nd"(port));
}

static inline void iowait() {
	outb(0x80, 0x00);
}

#endif // __BOLGENOS_NG__ASM_H__
