#ifndef __BOGLENOS_NG__MEM_UTILS_H__
#define __BOGLENOS_NG__MEM_UTILS_H__ (1)

static inline void __write_16(volatile uint16_t *addr, uint16_t data) {
	*addr = data;
}

static inline void write_16(uint16_t *addr, uint16_t data) {
	__write_16((volatile uint16_t*) addr, data);
}

static void memset_16(uint16_t *mem, uint16_t chunk, uint32_t size) {
	for(uint32_t pos = 0; pos != size; ++pos) {
		write_16(mem + pos, chunk);
	}
}

#endif // __BOGLENOS_NG__MEM_UTILS_H__
