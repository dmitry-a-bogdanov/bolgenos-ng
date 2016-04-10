#include <bolgenos-ng/asm.h>
#include <bolgenos-ng/cxxabi.h>
#include <bolgenos-ng/error.h>
#include <bolgenos-ng/time.h>

#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/pic_common.hpp>
#include <bolgenos-ng/pic_8259.hpp>
#include <bolgenos-ng/pit.hpp>
#include <bolgenos-ng/ps2.hpp>
#include <bolgenos-ng/slab.hpp>
#include <bolgenos-ng/vga_console.hpp>

#include <lib/ostream.hpp>

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

	mmu::init();	// Enables segmentation.
	memory::init(); // Allow allocation

	cio::cnotice << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< cio::endl;

	irq::init();

	pic::system_pic = &pic::chip_pic_8259;
	pic::system_pic->setup();

	pit::init();

	irq::enable();

	cio::cinfo << "CPU is initialized" << cio::endl;

	ps2::init();

	ost::run();

	do {
		asm ("hlt");
	} while(1);
}

