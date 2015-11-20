#include <bolgenos-ng/string.h>

#include <stdarg.h> // from gcc!

#include <bolgenos-ng/mem_utils.h>

static char __to_printable(int chr) {
	if (chr < 10) {
		return '0' + chr;
	} else if (chr >= 10) {
		return 'a' + (chr - 10);
	} else {
		return '_';
	}
}

const uint32_t		__uint32_max_divisor = 1000000000; // max = 4294967296

static int __snprintf_uint32(char *str, size_t size, uint32_t value);

int snprintf(char *str, size_t size, const char *format, ...) {
	va_list arg;
	va_start(arg, format);
	va_end(arg);
	// FIXME: just to remove compilation warning
	return __snprintf_uint32(str, size, 0);

}




static int __snprintf_uint32(char *str, size_t size, uint32_t value) {
	uint32_t divisor = __uint32_max_divisor;
	int leading_zero = 1;
	size_t written = 0;
	while (divisor != 0 && value != 0 && written < size) {
		int q = value / divisor;
		if (q)
			leading_zero = 0;
		if (q || !leading_zero) {
			char printable = __to_printable(q);
			str[written++] = printable;
		}
		value = value % divisor;
		divisor = divisor / 10;
	}
	if (written == 0) {
		str[written++] = '0';
	}
	return written;
}

