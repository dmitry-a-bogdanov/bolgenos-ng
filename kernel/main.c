#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/bolgenos-ng.h>
#include <bolgenos-ng/vga_console.h>
#include <bolgenos-ng/mmu.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/string.h>

#include "bootstrap.h"

multiboot_header_t
	__attribute__((section(".multiboot_header"), used))
	multiboot_header = mbh_initializer(MBH_ALIGN | MBH_MEMINFO);

void kernel_main() {
	interrupts_disable();
	vga_console_init();
	vga_clear_screen();

	vga_console_puts("Starting bolgenos-ng-" BOLGENOS_NG_VERSION "\n"); 

	setup_segments();
	setup_interrupts();

	interrupts_enable();

	vga_console_puts("CPU is initialized\n");



	do {
		asm ("hlt");
	} while(1);
}

