#include <bolgenos-ng/cxxabi.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/pit.hpp>
#include <bolgenos-ng/time.hpp>
#include <bolgenos-ng/ps2_controller.hpp>
#include <bolgenos-ng/vga_console.hpp>
#include <x86/cpu.hpp>
#include <x86/multitasking.hpp>
#include <x86/scheduler.hpp>

#include "config.h"

#include "traps.hpp"

using namespace lib;

x86::Processor cpu;
x86::Scheduler scheduler;


[[noreturn]] void task1() {
	cinfo << "Started task 1" << endl;
	irq::enable();
	uint32_t counter = 0;
	while (true) {
		cnotice << "task1. iteration #" << ++counter << endl;
		sleep_ms(300);
		scheduler.yield();
	}
}


[[noreturn]]
void multithreaded_init_stage() {
	cnotice << "Continue initialization in multithreaded env" << endl;
	cnotice << "Kernel initialization routine has been finished!"
	      << endl;
	irq::enable();

	scheduler.create_task(task1, "task #1");
	cinfo << "created first task" << endl;

	uint32_t counter = 0;

	do {
		x86::halt_cpu();
		scheduler.yield();
		cnotice << "main task. iteration #" << ++counter << endl;
	} while(true);
}

/**
* \brief Kernel main function.
*
* The main kernel function. The function performs full bootstrap of kernel
*	and then goes to idle state.
*/
extern "C" [[maybe_unused]] [[noreturn]] void kernel_main() {
	irq::disable(false);

	multiboot::init();

	call_global_ctors();

	set_log_level(log_level_type::notice);

	vga_console::clear_screen();

	cout
		<< R"(  __                           __  )" << endl
		<< R"( |__| _ | _  _  _  _  _  |\  |/    )" << endl
		<< R"( |  \| ||/ ||_|| || ||_  | \ ||  _ )" << endl
		<< R"( |__/|_||\_||_ | ||_| _| |  \|\__/ )" << endl
		<< R"(       ____|                       )" << endl;

	cnotice << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< endl;

	cpu.load_kernel_segments();
	memory::init(); // Allow allocation


	// explicitly create instance
	auto interrupt_manager = irq::InterruptsManager::instance();
	irq::install_traps();
	(void) interrupt_manager;



	auto interrupt_controller = devices::InterruptController::instance();
	interrupt_controller->initialize_controller();



	pit::init();

	irq::enable();

	cinfo << "CPU is initialized" << endl;

	ps2::PS2Controller::instance()->initialize_controller();

	ost::run();

	cwarn << "Kernel initialization routine has been finished!"
			<< endl;

	cwarn << "starting first switch" << endl;

	scheduler.init_multitasking(&cpu.gdt(), multithreaded_init_stage);

	//x86::switch_to(mmu::SegmentIndex::kernel_scheduler);

	panic("Couldn't switch to scheduler");

	do {
		x86::halt_cpu();
	} while(true);
}

