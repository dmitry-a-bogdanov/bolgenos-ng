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


#define out_n_as_type(type, n) \
	"(" #type ") " << (type) n << ", "

#define out_n_as_everything(n) \
	lib::cout << #n " as :" << out_n_as_type(char, n) \
		<< out_n_as_type(short, n) \
		<< out_n_as_type(int, n) \
		<< out_n_as_type(long, n) \
		<< out_n_as_type(unsigned char, n) \
		<< out_n_as_type(unsigned short, n) \
		<< out_n_as_type(unsigned int, n) \
		<< out_n_as_type(unsigned long, n) \
		<< lib::endl;

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

	vga_console::clear_screen();

	mmu::init();	// Enables segmentation.
	memory::init(); // Allow allocation

	lib::cnotice << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< lib::endl;

	irq::init();

	pic::system_pic = &pic::chip_pic_8259;
	pic::system_pic->setup();

	pit::init();

	irq::enable();

	lib::cinfo << "CPU is initialized" << lib::endl;

	ps2::init();

	ost::run();

	/*
	out_n_as_everything(0);
	out_n_as_everything(1);
	out_n_as_everything(-1);
	lib::cout	<< "test_min_val " << (char) -128 << ';'
			<< (short) -32768 << ';'
			<< (int) -2147483648 << ';'
			<< lib::endl;
			*/

	do {
		asm ("hlt");
	} while(1);
}

