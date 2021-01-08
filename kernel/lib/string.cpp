#include <bolgenos-ng/string.h>


size_t strlen(const char *str) {
	size_t length = 0;
	while (*(str++))
		++length;
	return length;
}

char *strcpy(char *dest, const char *src) {
	size_t pos = 0;
	do {
		dest[pos] = src[pos];
	} while(src[pos++]);
	return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
	size_t pos = 0;
	while (pos != n && src[pos]) {
		dest[pos] = src[pos];
		++pos;
	}
	while (pos != n) {
		dest[pos++] = '\0';
	}
	return dest;
}


void *memset(void *s, int c, size_t n) {
	const auto byte = static_cast<lib::byte>(c);
	auto* memory = static_cast<lib::byte *>(s);
	while (n--) {
		*(memory++) = byte;
	}
	return s;
}
