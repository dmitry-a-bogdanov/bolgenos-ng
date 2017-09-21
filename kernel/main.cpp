#include <ostream>

#include <bolgenos_config.hpp>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/cxxabi.h>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/init_queue.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/io/vga/text_console.hpp>

#include <bolgenos-ng/log.hpp>

#include "traps.hpp"


extern "C" void kernel_main() {
	irq::disable();

	multiboot::init();

	call_global_ctors();

	lib::set_log_level(lib::log_level_type::notice);

	bolgenos::io::vga::TextConsole::instance()->clear_screen();

	LOG_NOTICE("Starting bolgenos-ng-" << config::BOLGENOS_NG_VERSION);

	mmu::init();	// Enables segmentation.
	memory::init(); // Allow allocation
	irq::install_traps();

	auto interrupt_controller = devices::InterruptController::instance();
	interrupt_controller->initialize_controller();

	LOG_NOTICE("CPU is initialized");

	bolgenos::init::Queue::instance().execute();

	LOG_NOTICE("Kernel initialization routine has been finished!");

	do {
		x86::halt_cpu();
	} while(1);
}

