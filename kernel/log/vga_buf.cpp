#include "vga_buf.hpp"

#include <bolgenos-ng/vga_console.hpp>


VgaBuf::VgaBuf()
	: streambuf() {
}

VgaBuf::~VgaBuf() {
}

int VgaBuf::overflow(int c) {
	vga_console::putc(c);
	return c;
}

size_t VgaBuf::xsputn(const char* s, size_t n) {
	for (size_t cnt = 0; cnt != n; ++cnt) {
		overflow(s[cnt]);
	}
	return n;
}
