#include "vga_buf.hpp"

#include <bolgenos-ng/io/vga/text_console.hpp>


lib::_impl::vga_buf::vga_buf()
	: streambuf() {
}


lib::_impl::vga_buf::~vga_buf() {
}


int lib::_impl::vga_buf::overflow(int c) {
	bolgenos::io::vga::TextConsole::instance()->putc(c);
	return c;
}

size_t lib::_impl::vga_buf::xsputn(const char* s, size_t n) {
	for (size_t cnt = 0; cnt != n; ++cnt) {
		overflow(s[cnt]);
	}
	return n;
}
