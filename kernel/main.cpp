
#include <cxxabi.h>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/irq.hpp>
#include <log.hpp>
#include <logger.hpp>
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

using namespace lib;

x86::Processor cpu;

constexpr uint32_t sleep_interval = 1000;

LOCAL_LOGGER("main", LogLevel::INFO);

void test_task(void* number_ptr) {
	LOCAL_LOGGER("test_task", LogLevel::INFO);
	const int task_number = *static_cast<const int *>(number_ptr);
	LOG_INFO << "Started task " << task_number << endl;
	uint32_t counter = 0;
	if (task_number % 2) {
		while (true) {
			LOG_NOTICE << "task " << task_number << ". iteration #" << ++counter << endl;
			sleep_ms(sleep_interval);
		}
	} else {
		for (int i = 0; i < 5; ++i) {
			LOG_NOTICE << "task " << task_number << ". iteration #" << ++counter << endl;
			sleep_ms(sleep_interval);
		}
	}
}

[[noreturn]]
void multithreaded_init_stage(void*) {
	LOG_NOTICE << "Continue initialization in multithreaded env" << endl;
	LOG_NOTICE << "Configuring serial port" << endl;

	LOG_NOTICE << "Kernel initialization routine has been finished!" << endl;

	int tasks_count = 8;
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

	vga_console::clear_screen();

	LOG_WARN
		<< R"(  ____          _                                  _   _   ____  )" << endl
		<< R"( | __ )   ___  | |  __ _   ___  _ __    ___   ___ | \ | | / ___| )" << endl
		<< R"( |  _ \  / _ \ | | / _` | / _ \| '_ \  / _ \ / __||  \| || |  _  )" << endl
		<< R"( | |_) || (_) || || (_| ||  __/| | | || (_) |\__ \| |\  || |_| | )" << endl
		<< R"( |____/  \___/ |_| \__, | \___||_| |_| \___/ |___/|_| \_| \____| )" << endl
		<< R"(                   |___/                                         )" << endl
		;

	LOG_NOTICE << "Starting bolgenos-ng-" << BOLGENOS_NG_VERSION
		<< endl;

	cpu.load_kernel_segments();
	cpu.load_interrupts_table();
	memory::init(); // Allow allocation

	irq::InterruptsManager::init();

	auto interrupt_controller = devices::InterruptController::instance();
	interrupt_controller->initialize_controller();



	pit::init();

	irq::enable();

	LOG_INFO << "CPU is initialized" << endl;

	ps2::PS2Controller::instance()->initialize_controller();

	ost::run();

	LOG_WARN << "Kernel initialization routine has been finished!"
			<< endl;

	LOG_WARN << "starting first switch" << endl;

	sched::details::init_scheduling(multithreaded_init_stage);

	panic("Couldn't switch to scheduler");
}

