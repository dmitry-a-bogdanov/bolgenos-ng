#include <bolgenos-ng/bolgenos-ng.h>
#include <bolgenos-ng/vga_console.h>

#include "bootstrap.h"

multiboot_header_t
	__attribute__((section(".multiboot_header"), unused))
	multiboot_header = mbh_initializer(MBH_ALIGN | MBH_MEMINFO);

void kernel_main() {
	vga_console_init();
	vga_clear_screen();

	vga_console_puts("Starting bolgenos-ng-" BOLGENOS_NG_VERSION);
	do {
		asm ("hlt");
	} while(1);
}

