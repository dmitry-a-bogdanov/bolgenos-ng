#include <bolgenos-ng/error.h>

#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/vga_console.h>

void panic(const char *msg) {
	interrupts_disable();
	vga_console_puts("Kernel Panic:");
	vga_console_puts(msg);
	while(1) {
		halt_cpu();
	}
}

void bug(const char *msg) {
	vga_console_puts("Kernel Bug! Escalating to Kernel Panic...\n");
	panic(msg);
}
