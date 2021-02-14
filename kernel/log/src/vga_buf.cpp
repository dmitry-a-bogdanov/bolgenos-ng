#include <log/vga_buf.hpp>

#include <bolgenos-ng/vga_console.hpp>

int VgaBuf::overflow(int c) {
	vga_console::putc(c);
	return c;
}
