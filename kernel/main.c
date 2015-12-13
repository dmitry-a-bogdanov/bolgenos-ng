#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/bolgenos-ng.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.h>
#include <bolgenos-ng/mmu.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/pic_8259.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/printk.h>
#include <bolgenos-ng/ps2.h>
#include <bolgenos-ng/ps2_keyboard.h>
#include <bolgenos-ng/string.h>
#include <bolgenos-ng/time.h>
#include <bolgenos-ng/vga_console.h>

#include "bootstrap.h"


/**
* \brief Multiboot header.
*
* This symbols declares multiboot header for kernel. It must be placed into
*	separate section and this section must be placed by linker to the
*	beginning of resulting ELF-file.
*/
multiboot_header_t
	__attribute__((section(".multiboot_header"), used))
	multiboot_header = mbh_initializer(MBH_ALIGN | MBH_MEMINFO);


/**
* \brief Boot info from bootloader.
*
* Pointer to boot info structure provided by the bootloader. This symbol
* is set by assembler part of bootstrap code.
* \warning Data that is pointer by this symbol should be copied to kernel
*	internal memory before using memory allocation!
*/
boot_info_t *__boot_loader_boot_info;


/**
* \brief Boot info structure.
*
* This structure is an own instance of boot info provided by bootloader.
* Structure has valid value only after calling extract_boot_info.
*/
boot_info_t boot_info;

/**
* \brief Extract boot info.
*
* Copy boot information structure provided by Multiboot bootloader to
*	internal kernel memory.
*/
void extract_boot_info() {
	memcpy(&boot_info, __boot_loader_boot_info, sizeof(boot_info_t));
}


/**
* \brief Kernel main function.
*
* The main kernel function. The fuction performs full bootstrap of kernel
*	and then goes to idle state.
*/
void kernel_main() {
	interrupts_disable();

	extract_boot_info();

	vga_console_init();
	vga_clear_screen();

	printk("Starting bolgenos-ng-" BOLGENOS_NG_VERSION "\n");
	if (boot_info.flags & MBI_MEM_INFO) {
		printk("Detected memory: low = %lu kb, high = %lu kb\n",
			(long unsigned) boot_info.mem_lower,
			(long unsigned) boot_info.mem_upper);
	} else {
		panic("Bootloader didn't provide memory info!\n");
	}

	setup_segments();
	setup_interrupts();

	system_pic = &pic_8259;
	system_pic->setup();

	init_timer();

	interrupts_enable();

	printk("CPU is initialized\n");

	ps2_keyboard_init();
	ps2_init();

	do {
		asm ("hlt");
	} while(1);
}

