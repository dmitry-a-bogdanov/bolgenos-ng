#include <bolgenos-ng/cxxabi.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/memory.hpp>
#include <bolgenos-ng/multiboot_info.hpp>
#include <bolgenos-ng/ost.hpp>
#include <bolgenos-ng/pit.hpp>
#include <bolgenos-ng/time.hpp>
#include <ps2/controller.hpp>
#include <bolgenos-ng/vga_console.hpp>
#include <x86/cpu.hpp>
#include <sched.hpp>

#include "config.h"

#include "traps.hpp"

using namespace lib;

x86::Processor cpu;

constexpr uint32_t sleep_interval = 500;

[[noreturn]] void test_task(void* number_ptr) {
	const int task_number = *static_cast<const int *>(number_ptr);
	cinfo << "Started task " << task_number << endl;
	uint32_t counter = 0;
	while (true) {
		cnotice << "task " << task_number << ". iteration #" << ++counter << endl;
		sleep_ms(sleep_interval);
	}
}


[[noreturn]]
void multithreaded_init_stage(void*) {
	cnotice << "Continue initialization in multithreaded env" << endl;
	cnotice << "Kernel initialization routine has been finished!"
	      << endl;
	irq::enable();

	int tasks_count = 5;
	for (int i = 0; i < tasks_count; ++i) {
		sched::create_task(test_task, new int{i}, "test_task");
		sleep_ms(sleep_interval/tasks_count);
	}

	do {
		x86::halt_cpu();
		sched::yield();
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
		<< R"(  ____          _                                  _   _   ____  )" << endl
		<< R"( | __ )   ___  | |  __ _   ___  _ __    ___   ___ | \ | | / ___| )" << endl
		<< R"( |  _ \  / _ \ | | / _` | / _ \| '_ \  / _ \ / __||  \| || |  _  )" << endl
		<< R"( | |_) || (_) || || (_| ||  __/| | | || (_) |\__ \| |\  || |_| | )" << endl
		<< R"( |____/  \___/ |_| \__, | \___||_| |_| \___/ |___/|_| \_| \____| )" << endl
		<< R"(                   |___/                                         )" << endl
		;

	cnotice << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< endl;

	cpu.load_kernel_segments();
	cpu.load_interrupts_table();
	memory::init(); // Allow allocation


	// explicitly create instance
	irq::InterruptsManager::init();
	irq::install_traps();



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

	sched::details::init_scheduling(multithreaded_init_stage);

	panic("Couldn't switch to scheduler");
}

