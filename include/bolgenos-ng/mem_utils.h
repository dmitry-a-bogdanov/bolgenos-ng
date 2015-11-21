#ifndef __BOGLENOS_NG__MEM_UTILS_H__
#define __BOGLENOS_NG__MEM_UTILS_H__ (1)

#ifndef NULL
#define NULL ((void *)0)
#endif

#define address_of(var) ((char *)&var)

#define check_type_size(type, expected_size)				\
	static char ___ ## type ## _size_assertion_1[sizeof(type) -	\
		expected_size] __attribute__((unused));			\
	static char ___ ## type ## _size_assertion_2[expected_size -	\
		sizeof(type)] __attribute__((unused))

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

static inline void memset(void *mem, char val, size_t size) {
	char *__mem = (char *) mem;
	for(size_t pos = 0; pos != size; ++pos, ++mem) {
		write_8(__mem, &val);
	}
}

static inline void memset_16(char* mem, char* chunk, size_t size) {
	for(size_t pos = 0; pos != size; ++pos, mem += 2) {
		write_16(mem, chunk);
	}
}

#define bitmask(value, offset, mask) (((value) >> (offset)) & (mask))

static inline void memcpy(void *dest, const void *src, size_t size) {
	char *__dest = (char *) dest;
	char *__src = (char* ) src;
	for (size_t pos = 0; pos != size; ++pos) {
		__dest[pos] = __src[pos];
	}
}

#endif // __BOGLENOS_NG__MEM_UTILS_H__
