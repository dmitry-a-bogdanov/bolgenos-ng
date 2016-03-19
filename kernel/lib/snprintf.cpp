#include <bolgenos-ng/string.h>

#include <stdarg.h> // from gcc!

#include <bolgenos-ng/error.h>

#include <bolgenos-ng/mem_utils.hpp>

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
const int32_t		__int32_max_divisor = 1000000000;

static int __snprintf_int32(char *str, size_t size, int32_t value);	// %li
static int __snprintf_uint32(char *str, size_t size, uint32_t value);	// %lu

enum fmt_spec {
	fmt_s_copy,
	fmt_s_string,
	fmt_s_long,
	fmt_s_ulong
};

struct format_entry {
	enum fmt_spec spec;
};

#define __SPEC_SYMBOL		('%')

#define __SPEC_LONG		('l')

#define __SPEC_UNSIGNED		('u')
#define __SPEC_INTEGER		('i')
#define __SPEC_DECIMAL		('d')
#define __SPEC_STRING		('s')

#define __is_eos(chr)		((chr) == '\0')
#define __is_spec_sym(chr)	((chr) == __SPEC_SYMBOL)

#define __FMT_ERROR		(-1)
#define __FMT_END		(0)

static int parse_specifier(const char *fmt, struct format_entry *ret) {
	switch (*fmt) {
	case __SPEC_LONG:
		switch (*++fmt) {
		case __SPEC_UNSIGNED:
			ret->spec = fmt_s_ulong;
			return 2;
		case __SPEC_DECIMAL:
		case __SPEC_INTEGER:
			ret->spec = fmt_s_long;
			return 2;
		default:
			return __FMT_ERROR;
		}
	case __SPEC_STRING:
		ret->spec = fmt_s_string;
		return 1;
	default:
		// Format is incorrect
		return __FMT_ERROR;
	}
}

static int read_format(const char *fmt, struct format_entry *ret) {
	switch (*fmt) {
	case '\0':
		return __FMT_END;
	case __SPEC_SYMBOL:
		{
			// Current format starts with specifier
			int length = parse_specifier(fmt + 1, ret);
			if (length != __FMT_ERROR)
				length += 1;
			return length;
		}
	default:
		{
			int length = 0;
			while ((!__is_eos(*fmt)) && (!__is_spec_sym(*fmt))) {
				++fmt;
				++length;
			}
			ret->spec = fmt_s_copy;
			return length;
		}
	}
}

int snprintf(char *str, size_t size, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vsnprintf(str, size, format, args);
	va_end(args);

	return ret;
}

int vsnprintf(char *str, size_t size, const char *format, va_list arg) {
	int written = 0;
	while (*format && size > 1) {
		struct format_entry entry;
		int fmt_entry_length = read_format(format, &entry);

		switch (fmt_entry_length) {
		case __FMT_ERROR:
			goto out_fail;
		case __FMT_END:
			goto out;
		}

		switch (entry.spec) {
		case fmt_s_copy:
			{
				size_t print_chars = fmt_entry_length;
				if ((size_t) print_chars >= size - 1) {
					print_chars = size - 1;
					// one char is reserved for null byte
				}
				strncpy(str, format, print_chars);
				size -= print_chars;
				str += print_chars;
				written += print_chars;
				break;
			}
		case fmt_s_string:
			{
				char *string = va_arg(arg, char *);
				int print_chars = strlen(string);
				if ((size_t) print_chars >= size - 1) {
					print_chars = size - 1;
				}
				strncpy(str, string, print_chars);
				size -= print_chars;
				str += print_chars;
				written += print_chars;
				break;
			}
		case fmt_s_long:
			{
				int32_t val = va_arg(arg, int32_t);
				int print_chars =
					__snprintf_int32(str, size - 1, val);
				size -= print_chars;
				str += print_chars;
				written += print_chars;
				break;
			}
		case fmt_s_ulong:
			{
				uint32_t val = va_arg(arg, uint32_t);
				int print_chars =
					__snprintf_uint32(str, size - 1, val);
				size -= print_chars;
				str += print_chars;
				written += print_chars;
				break;
			}
		default:
			goto out_fail;
		}

		format += fmt_entry_length;
	}
out:
	*str = '\0';
	return written;

out_fail:
	bug("out_fail reached in snprintf");
}

static int __snprintf_int32(char *str, size_t size, int32_t value) {
	if (str == NULL || size == 0)
		return 0;
	int written = 0;

	// write sign if needed
	if (value < 0)
		str[written++] = '-';

	uint32_t unsigned_value;
	if (value < 0) {
		unsigned_value = ((uint32_t) 0) - value;
	} else {
		unsigned_value = value;
	}
	return written + __snprintf_uint32(str + written, size - written,
			unsigned_value);
}


static int __snprintf_uint32(char *str, size_t size, uint32_t value) {
	if (str == NULL || size == 0)
		return 0;
	uint32_t divisor = __uint32_max_divisor;
	int leading_zero = 1;
	size_t written = 0;
	while (divisor != 0 && written < size) {
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

