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
#include <bolgenos-ng/traps.hpp>
#include <bolgenos-ng/vga_console.hpp>

#include <lib/ostream.hpp>

#include <bolgenos-ng/execinfo.hpp>

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
	irq::install_traps();


	pic::system_pic = &pic::chip_pic_8259;
	pic::system_pic->setup();

	pit::init();

	irq::enable();

	lib::cinfo << "CPU is initialized" << lib::endl;

	ps2::init();

	ost::run();

	asm (
		"mov $0xdead000a, %eax\n"
		"mov $0xbeef000b, %ebx\n"
		"mov $0xb00b000c, %ecx\n"
		"mov $0xc0ffee0d, %edx\n"
		"mov $0x51515151, %esi\n"
		"mov $0xd1d1d1d1, %edi\n"
		"int $3\n");

	do {
		asm ("hlt");
	} while(1);
}

