#ifndef __BOGLENOS_NG__MEM_UTILS_H__
#define __BOGLENOS_NG__MEM_UTILS_H__ (1)

#define address_of(var) ((char *)&var)

static inline void __write_8(volatile char *dest, char *src) {
	*dest = *src;
}

static inline void write_8(char *dest, char *src) {
	__write_8((volatile char *) dest, src);
}

static inline void write_16(char *dest, char *src) {
	write_8(dest, src);
	write_8(dest + 1, src + 1);
}

static inline void memset_16(char* mem, char* chunk, uint32_t size) {
	for(uint32_t pos = 0; pos != size; ++pos, mem += 2) {
		write_16(mem, chunk);
	}
}

#endif // __BOGLENOS_NG__MEM_UTILS_H__
