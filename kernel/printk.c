#include <bolgenos-ng/printk.h>

#include <bolgenos-ng/string.h>
#include <bolgenos-ng/vga_console.h>
#define __printk_buf_size			(1024)

int printk(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vprintk(fmt, args);
	va_end(args);

	return ret;
}

int vprintk(const char *fmt, va_list args) {
	char buf[__printk_buf_size] = { 0 };
	int ret = vsnprintf(buf, __printk_buf_size, fmt, args);
	vga_console_puts(buf);
	return ret;
}
