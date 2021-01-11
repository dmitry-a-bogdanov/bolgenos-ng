#include <bolgenos-ng/irq.hpp>

#include <algorithm.hpp>

#include <bolgenos-ng/error.h>
#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>

#include <ext/scoped_format_guard.hpp>

#include <m4/idt.hpp>
#include <atomic.hpp>

irq::InterruptsManager *irq::InterruptsManager::_instance = nullptr;

irq::InterruptsManager::InterruptsManager()
{
	idt_pointer.base = m4::get_idt(handle_irq);
	uint16_t idt_size = irq::NUMBER_OF_LINES*irq::GATE_SIZE - 1;
	idt_pointer.limit = idt_size;
	asm volatile("lidt %0"::"m" (idt_pointer));
}

irq::InterruptsManager *irq::InterruptsManager::instance()
{
	if (!_instance) {
		_instance = new InterruptsManager{};
	}
	return _instance;
}


void irq::InterruptsManager::add_handler(irq_t vector, IRQHandler *handler)
{
	auto& handlers_list = _irq_handlers[vector];
	if (handlers_list.push_front(handler) == handlers_list.end()) {
		panic("failed to register interrupt handler");
	}
}


void irq::InterruptsManager::add_handler(exception_t exception, ExceptionHandler *handler)
{
	auto& handlers_list = _exceptions_handlers[exception];
	if (handlers_list.push_front(handler) == handlers_list.end()) {
		panic("failed to register exception handler");
	}
}


bool irq::InterruptsManager::is_exception(irq::irq_t vector)
{
	return vector < exception_t::max;
}


irq::IRQHandler::status_t irq::InterruptsManager::dispatch_exception(exception_t exception,
		stack_ptr_t frame_pointer)
{
	auto& handlers = _exceptions_handlers[exception];

	if (handlers.empty()) {
		return irq::IRQHandler::status_t::NONE;
	}

	lib::for_each(handlers.begin(), handlers.end(),
		[frame_pointer](irq::ExceptionHandler *handler) -> void {
			handler->handle_exception(frame_pointer);
	});

	return irq::IRQHandler::status_t::HANDLED;
}


irq::IRQHandler::status_t irq::InterruptsManager::dispatch_interrupt(irq_t vector)
{
	auto& handlers = _irq_handlers[vector];
	auto used_handler = lib::find_if(handlers.begin(), handlers.end(),
		[vector] (irq::IRQHandler* handler) -> bool {
			return handler->handle_irq(vector) == irq::IRQHandler::status_t::HANDLED;
	});
	if (used_handler == handlers.end()) {
		return irq::IRQHandler::status_t::NONE;
	}
	return irq::IRQHandler::status_t::HANDLED;

}


void irq::InterruptsManager::handle_irq(irq_t vector, void *frame)
{
	irq::IRQHandler::status_t status;

	auto manager = irq::InterruptsManager::instance();
	if (is_exception(vector)) {
		status = manager->dispatch_exception(static_cast<exception_t>(vector), frame);
	} else {
		status = manager->dispatch_interrupt(vector);
	}

	if (status != irq::IRQHandler::status_t::HANDLED) {
		lib::ccrit << "Unhandled IRQ" << vector << lib::endl;
		panic("Fatal interrupt");
	}

	devices::InterruptController::instance()->end_of_interrupt(vector);
}




lib::ostream& irq::operator <<(lib::ostream& out,
		const irq::registers_dump_t& regs)
{
	lib::ScopedFormatGuard format_guard(out);

	out	<< lib::setw(0) << lib::hex << lib::setfill(' ');
	out	<< " eax = "
			<< lib::setw(8) << lib::setfill('0')
				<< regs.eax
			<< lib::setfill(' ') << lib::setw(0)
		<< ' '
		<< " ebx = " << lib::setw(8) << regs.ebx << lib::setw(0) << ' '
		<< " ecx = " << lib::setw(8) << regs.ecx << lib::setw(0) << ' '
		<< " edx = " << lib::setw(8) << regs.edx << lib::setw(0) << ' '
		<< lib::endl
		<< " esi = " << lib::setw(8) << regs.esi << lib::setw(0) << ' '
		<< " edi = " << lib::setw(8) << regs.edi << lib::setw(0) << ' '
		<< " ebp = " << lib::setw(8) << regs.ebp << lib::setw(0) << ' '
		<< " esp = " << lib::setw(8) << regs.esp << lib::setw(0) << ' ';

	return out;
}


lib::ostream& irq::operator <<(lib::ostream& out,
		const irq::execution_info_dump_t& exe)
{
	lib::ScopedFormatGuard format_guard(out);

	out	<< lib::setw(0) << lib::hex
		<< "eflg = " << lib::setw(8) << exe.eflags << lib::setw(0) << ' '
		<< "  cs = " << lib::setw(8) << exe.cs << lib::setw(0) << ' '
		<< " eip = " << lib::setw(8) << exe.eip << lib::setw(0);

	return out;
}


lib::ostream& irq::operator <<(lib::ostream& out,
		const irq::int_frame_error_t& frame)
{
	lib::ScopedFormatGuard format_guard(out);

	out	<< lib::hex
		<< lib::setw(0) << " err = "
		<< lib::setw(8) << frame.error_code
		<< lib::endl
		<< frame.exe << lib::endl
		<< frame.regs;

	return out;
}


lib::ostream& irq::operator <<(lib::ostream& out,
		const irq::int_frame_noerror_t& frame)
{
	lib::ScopedFormatGuard format_guard(out);

	out	<< lib::setw(0) << lib::hex
		<< frame.exe << lib::endl
		<< frame.regs;

	return out;
}

static lib::atomic<bool> interrupts_enabled{false};

void irq::enable() {
	lib::cnotice << "enabling interrupts" << lib::endl;
	interrupts_enabled.store(true);
	asm volatile ("sti\n");
}

bool irq::disable(bool debug) {
	if (debug) {
		lib::cnotice << "disabling interrupts" << lib::endl;
	}

	asm volatile ("cli\n");

	return interrupts_enabled.exchange(false);
}

// Compile-time guards
static_assert(sizeof(irq::registers_dump_t) == 8*4,
	"Wrong size of registers' dump structure");


static_assert(sizeof(irq::execution_info_dump_t) == 4 + 2 + 4,
	"Wrong size of execution info structure");


static_assert(sizeof(irq::int_frame_noerror_t) ==
	sizeof(irq::registers_dump_t) + sizeof(irq::execution_info_dump_t),
	"Wrong size of interrupt frame (no error code)");


static_assert(sizeof(irq::int_frame_error_t) ==
	sizeof(irq::int_frame_noerror_t) + 4,
	"Wrong size of interrupt frame (error code)");

