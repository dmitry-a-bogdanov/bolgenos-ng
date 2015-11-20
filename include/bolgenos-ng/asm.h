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

static inline uint8_t inb(uint16_t port) {
	uint8_t byte;
	asm volatile("inb %1, %0": "=a"(byte) : "Nd"(port));
	return byte;
}

static inline void iowait() {
	outb(0x80, 0x00);
}

static inline void cpuid(uint32_t eax, uint32_t *edx, uint32_t *ecx) {
	asm volatile("cpuid": "=d"(*edx), "=c"(*ecx): "a"(eax));
}

// Model Specific Register
typedef enum {
	msr_ia32_apic_base = 0x1b
} msr_t;

static inline void read_msr(msr_t msr, uint32_t *low, uint32_t *high) {
	asm volatile("rdmsr": "=a"(*low), "=d"(*high): "c"(msr));
}

static inline void write_msr(msr_t msr, uint32_t low, uint32_t high) {
	asm volatile("wrmsr":: "a"(low), "d"(high), "c"(msr));
}

#endif // __BOLGENOS_NG__ASM_H__
