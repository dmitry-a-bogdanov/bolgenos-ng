#include <bolgenos-ng/cxxabi.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/ost.hpp>
#include <m4/idt.hpp>
#include <bolgenos-ng/pit.hpp>
#include <bolgenos-ng/ps2_controller.hpp>
#include <bolgenos-ng/vga_console.hpp>

#include <arch/x86/multitasking.hpp>

#include <lib/ostream.hpp>

#include "config.h"

#include "traps.hpp"

/**
* \brief Kernel main function.
*
* The main kernel function. The function performs full bootstrap of kernel
*	and then goes to idle state.
*/
extern "C" [[noreturn]] void kernel_main() {
	irq::disable(false);

	multiboot::init();

	call_global_ctors();

	lib::set_log_level(lib::log_level_type::notice);

	vga_console::clear_screen();

	mmu::init();	// Enables segmentation.
	memory::init(); // Allow allocation



	lib::cnotice << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< lib::endl;

	// explicitly create instance
	auto interrupt_manager = irq::InterruptsManager::instance();
	irq::install_traps();
	(void) interrupt_manager;



	auto interrupt_controller = devices::InterruptController::instance();
	interrupt_controller->initialize_controller();



	pit::init();

	irq::enable();

	lib::cinfo << "CPU is initialized" << lib::endl;

	ps2::PS2Controller::instance()->initialize_controller();

	ost::run();

	lib::cwarn << "Kernel initialization routine has been finished!"
			<< lib::endl;

	//irq::disable();

	lib::cwarn << "starting first switch" << lib::endl;
	lib::cnotice
		<< "1:" << other_task << lib::endl
		<< "2:" << other_task2 << lib::endl;

	x86::switch_to(mmu::SegmentIndex::kernel_scheduler);

	/*
	struct {unsigned int offset; unsigned short segment;} dest;
	dest.offset = 0x0;
	dest.segment = target; // whatever value you want in CS
	asm volatile (
	//"movl $1f, %0\n"
	"ljmp *%0\n"
	"1:" :: "m"(dest));
	 */

	/*
	asm volatile(
	"pushw $32\n\t"
 	"pushl $0\n\t"
  	"ljmp *%%esp\n\t"
	:
	: "r"(&target)
	:
	);*/
	/*asm volatile(
			"movw $32, %%ax\n\t"
			"ltr %%ax\n\t"
	      		"ret\n\t"
	      		::: "cc");*/

	lib::cwarn << "Switched!"
		   << lib::endl;


	gate_t* current_task_gate;

	asm volatile("str %0\n\t"
		: "=m"(current_task_gate)
		:
		: "memory");

	lib::cnotice << "task gate ptr: " << current_task_gate << lib::endl;

	do {
		x86::halt_cpu();
	} while(true);
}

