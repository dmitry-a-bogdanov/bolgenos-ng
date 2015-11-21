#include <bolgenos-ng/error.h>

#include <bolgenos-ng/vga_console.h>

void panic() {
	vga_console_puts("Panic!");
	asm volatile("cli");
	while(1) {
		asm volatile("hlt");
	}
}

void bug() {
	panic();
}
