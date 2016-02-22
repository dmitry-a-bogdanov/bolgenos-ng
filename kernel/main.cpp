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
#include <bolgenos-ng/slab.h>
#include <bolgenos-ng/time.h>
#include <bolgenos-ng/vga_console.h>

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

	struct slab_area test_slab;
	if (slab_init(&test_slab, sizeof(long), 10) != SLAB_OK) {
		panic("slab initilization failure\n");
	}
	void *one_page = slab_alloc(&test_slab);
	void *two_page = slab_alloc(&test_slab);
	void *three_page = slab_alloc(&test_slab);
	void *one_page_2 = slab_alloc(&test_slab);
	slab_free(&test_slab, two_page);
	void *two_page_2 = slab_alloc(&test_slab);

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

