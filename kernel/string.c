#include <bolgenos-ng/string.h>

static char chr_to_printable(int chr) {
	if (chr < 10) {
		return '0' + chr;
	} else if (chr >= 10) {
		return 'a' + (chr - 10);
	} else {
		return '_';
	}
}

static void swap(char *a, char *b) {
	char tmp = *a;
	*a = *b;
	*b = tmp;
}

uint32_t uint32_to_string(uint32_t val, char *str, unsigned int base) {
	char *end = str;
	uint32_t chrs_written = 0;
	while(val != 0) {
		int chr = val % base;
		val = val / base;
		*end++ = chr_to_printable(chr);
		++chrs_written;
	}
	if (chrs_written == 0) {
		*end++ = '0';
		++chrs_written;
	};
	end -= 1;
	while (str < end) {
		swap(str, end);
		++str;
		--end;
	}
	return chrs_written;
}
