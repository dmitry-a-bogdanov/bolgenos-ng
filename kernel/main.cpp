#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/cxxabi.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/mem_utils.h>
#include <bolgenos-ng/mmu.h>
#include <bolgenos-ng/pic_8259.h>
#include <bolgenos-ng/pic_common.h>
#include <bolgenos-ng/time.h>

#include <bolgenos-ng/cout.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/pit.hpp>
#include <bolgenos-ng/ps2.hpp>
#include <bolgenos-ng/slab.hpp>
#include <bolgenos-ng/vga_console.hpp>

#include "config.h"

/**
* \brief Kernel main function.
*
* The main kernel function. The fuction performs full bootstrap of kernel
*	and then goes to idle state.
*/
extern "C" void kernel_main() {
	irq::disable();

	multiboot::init();

	call_global_ctors();

	vga_console::clear_screen();

	memory::init(); // Allow allocation

	cio::cnotice << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< cio::endl;


	setup_interrupts();
	setup_segments();

	system_pic = &pic_8259;
	system_pic->setup();

	pit::init();

	irq::enable();

	cio::cinfo << "CPU is initialized" << cio::endl;

	ps2::init();

	ost::run();

	do {
		asm ("hlt");
	} while(1);
}

