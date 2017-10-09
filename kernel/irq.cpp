#include <bolgenos-ng/irq.hpp>

#include <algorithm>
#include <forward_list>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/error.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/mem_utils.hpp>

#include <basalt/format_guard.hpp>

#include <m4/idt.hpp>

irq::InterruptsManager *irq::InterruptsManager::_instance = nullptr;


namespace
{

bolgenos::irq::dispatcher_function_t *dispatcher;

}


void bolgenos::irq::set_dispatcher(dispatcher_function_t *f)
{
	dispatcher = f;
}


bolgenos::irq::dispatcher_function_t* bolgenos::irq::get_dispatcher()
{
	return dispatcher;
}


irq::InterruptsManager::InterruptsManager()
{
	bolgenos::irq::set_dispatcher(handle_irq);
	idt_pointer.base = bolgenos::irq::idt;
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
	_irq_handlers[vector].push_front(handler);
}


void irq::InterruptsManager::add_handler(exception_t exception, ExceptionHandler *handler)
{
	_exceptions_handlers[exception].push_front(handler);
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

	std::for_each(handlers.begin(), handlers.end(),
		[frame_pointer](irq::ExceptionHandler *handler) -> void {
			handler->handle_exception(frame_pointer);
	});

	return irq::IRQHandler::status_t::HANDLED;
}


irq::IRQHandler::status_t irq::InterruptsManager::dispatch_interrupt(irq_t vector)
{
	auto& handlers = _irq_handlers[vector];
	auto used_handler = std::find_if(handlers.begin(), handlers.end(),
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
		LOG_CRITICAL("Unhandled IRQ" << vector);
		panic("Fatal interrupt");
	}

	devices::InterruptController::instance()->end_of_interrupt(vector);
}




std::ostream& irq::operator <<(std::ostream& out,
		const irq::registers_dump_t& regs)
{
	basalt::scoped_format_guard format_guard(out);

	out	<< std::setw(0) << std::hex << std::setfill(' ');
	out	<< " eax = "
			<< std::setw(8) << std::setfill('0')
				<< regs.eax
			<< std::setfill(' ') << std::setw(0)
		<< ' '
		<< " ebx = " << std::setw(8) << regs.ebx << std::setw(0) << ' '
		<< " ecx = " << std::setw(8) << regs.ecx << std::setw(0) << ' '
		<< " edx = " << std::setw(8) << regs.edx << std::setw(0) << ' '
		<< std::endl
		<< " esi = " << std::setw(8) << regs.esi << std::setw(0) << ' '
		<< " edi = " << std::setw(8) << regs.edi << std::setw(0) << ' '
		<< " ebp = " << std::setw(8) << regs.ebp << std::setw(0) << ' '
		<< " esp = " << std::setw(8) << regs.esp << std::setw(0) << ' ';

	return out;
}


std::ostream& irq::operator <<(std::ostream& out,
		const irq::execution_info_dump_t& exe)
{
	basalt::scoped_format_guard format_guard(out);

	out	<< std::setw(0) << std::hex
		<< "eflg = " << std::setw(8) << exe.eflags << std::setw(0) << ' '
		<< "  cs = " << std::setw(8) << exe.cs << std::setw(0) << ' '
		<< " eip = " << std::setw(8) << exe.eip << std::setw(0);

	return out;
}


std::ostream& irq::operator <<(std::ostream& out,
		const irq::int_frame_error_t& frame)
{
	basalt::scoped_format_guard format_guard(out);

	out	<< std::hex
		<< std::setw(0) << " err = "
		<< std::setw(8) << frame.error_code
		<< std::endl
		<< frame.exe << std::endl
		<< frame.regs;

	return out;
}


std::ostream& irq::operator <<(std::ostream& out,
		const irq::int_frame_noerror_t& frame)
{
	basalt::scoped_format_guard format_guard(out);

	out	<< std::setw(0) << std::hex
		<< frame.exe << std::endl
		<< frame.regs;

	return out;
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

