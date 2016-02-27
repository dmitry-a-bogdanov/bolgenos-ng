#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/cxxabi.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/mmu.h>
#include <bolgenos-ng/multiboot_info.h>
#include <bolgenos-ng/pic_8259.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/time.h>
#include <bolgenos-ng/vga_console.hpp>

#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/pit.hpp>
#include <bolgenos-ng/ps2.hpp>
#include <bolgenos-ng/slab.hpp>

#include "config.h"

/**
* \brief Kernel main function.
*
* The main kernel function. The fuction performs full bootstrap of kernel
*	and then goes to idle state.
*/
extern "C" void kernel_main() {
	interrupts_disable();

	multiboot_info_init();

	call_global_ctors();

	vga_console::init();
	vga_console::clear_screen();

	printk("Starting bolgenos-ng-" BOLGENOS_NG_VERSION "\n");


	setup_interrupts();
	setup_segments();

	system_pic = &pic_8259;
	system_pic->setup();

	pit::init();

	interrupts_enable();

	printk("CPU is initialized\n");

	memory::init();

	ps2::init();

	ost::run();

	do {
		asm ("hlt");
	} while(1);
}

