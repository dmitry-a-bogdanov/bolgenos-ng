#include <bolgenos-ng/int_types.h>

#include "bootstrap.h"

multiboot_header_t
	__attribute__((section(".multiboot_header"), unused))
	multiboot_header = mbh_initializer(MBH_ALIGN | MBH_MEMINFO);

void kernel_main() {
	do {
		asm ("hlt");
	} while(1);
}

