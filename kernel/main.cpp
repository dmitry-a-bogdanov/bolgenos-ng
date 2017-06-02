#include <bolgenos-ng/cxxabi.h>
#include <bolgenos-ng/error.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/pic_common.hpp>
#include <bolgenos-ng/pic_8259.hpp>
#include <bolgenos-ng/pit.hpp>
#include <bolgenos-ng/ps2_controller.hpp>
#include <bolgenos-ng/slab.hpp>
#include <bolgenos-ng/time.hpp>
#include <bolgenos-ng/vga_console.hpp>

#include <lib/ostream.hpp>

#include "config.h"

/**
* \brief Kernel main function.
*
* The main kernel function. The function performs full bootstrap of kernel
*	and then goes to idle state.
*/
extern "C" void kernel_main() {
	irq::disable();

	multiboot::init();

	call_global_ctors();

	lib::set_log_level(lib::log_level_type::notice);

	vga_console::clear_screen();

	mmu::init();	// Enables segmentation.
	memory::init(); // Allow allocation

	lib::cnotice << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< lib::endl;

	irq::init();

	auto pic = pic::pic_device::instance();
	pic->setup();

	pit::init();

	irq::enable();

	lib::cinfo << "CPU is initialized" << lib::endl;

	ps2::PS2Controller::instance()->initialize_controller();

	ost::run();

	lib::cwarn << "Kernel initialization routine has been finished!"
			<< lib::endl;
	do {
		x86::halt_cpu();
	} while(1);
}

