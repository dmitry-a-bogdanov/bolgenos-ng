#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/memory.h>
#include <bolgenos-ng/mmu.h>
#include <bolgenos-ng/multiboot_info.h>
#include <bolgenos-ng/pic_8259.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/pit.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/ps2.h>
#include <bolgenos-ng/ps2_keyboard.h>
#include <bolgenos-ng/time.h>
#include <bolgenos-ng/vga_console.h>

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

	vga_console_init();
	vga_clear_screen();

	printk("Starting bolgenos-ng-" BOLGENOS_NG_VERSION "\n");

	setup_interrupts();
	setup_segments();

	system_pic = &pic_8259;
	system_pic->setup();

	init_pit();

	interrupts_enable();

	printk("CPU is initialized\n");

	init_memory();

	slab_area test_slab(sizeof(long), 10);
	if (!test_slab.initialized()) {
		panic("slab initilization failure\n");
	}
	void *one_page = test_slab.allocate();
	void *two_page = test_slab.allocate();
	void *three_page = test_slab.allocate();
	void *one_page_2 = test_slab.allocate();
	test_slab.deallocate(two_page);
	void *two_page_2 = test_slab.allocate();

	printk("[MEM_TEST]: addresses: %lu, %lu==%lu, %lu, %lu\n",
		(unsigned long) one_page,
		(unsigned long) two_page,
		(unsigned long) two_page_2,
		(unsigned long) three_page,
		(unsigned long) one_page_2);

	ps2_keyboard_init();
	ps2_init();

	do {
		asm ("hlt");
	} while(1);
}
