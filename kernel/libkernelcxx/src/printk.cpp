#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/vga_console.hpp>

void printk(const char *str) {
	vga_console::puts(str);
}
