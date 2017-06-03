#include <bolgenos-ng/irq.hpp>

#include <bolgenos-ng/compiler.h>
#include <bolgenos-ng/error.h>

#include <bolgenos-ng/asm.hpp>
#include <bolgenos-ng/execinfo.hpp>
#include <bolgenos-ng/interrupt_controller.hpp>
#include <bolgenos-ng/mem_utils.hpp>
#include <bolgenos-ng/mmu.hpp>
#include <bolgenos-ng/stdtypes.hpp>

#include <lib/algorithm.hpp>
#include <lib/forward_list.hpp>

#include <m4/idt.hpp>

#include "traps.hpp"


namespace {


/// Array of lists of Interrupt Service Routines.
lib::forward_list<irq::irq_handler_t> irq_handlers[irq::NUMBER_OF_LINES];


/// Array of lists of Exception Handlers.
lib::forward_list<irq::exception_handler_t> exc_handlers[static_cast<int>(irq::exception_t::max)];


table_pointer idt_pointer _irq_aligned_;


void irq_dispatcher(irq::irq_t vector, void* frame);


} // namespace


void irq::init() {
	idt_pointer.base = m4::get_idt(irq_dispatcher);
	uint16_t idt_size = irq::NUMBER_OF_LINES*irq::GATE_SIZE - 1;
	idt_pointer.limit = idt_size;
	asm volatile("lidt %0"::"m" (idt_pointer));
	irq::install_traps();

}


void irq::request_irq(irq_t vector, irq_handler_t routine) {
	auto& handlers_list = irq_handlers[vector];
	if (handlers_list.push_front(routine) == handlers_list.end()) {
		panic("failed to register interrupt handler");
	}
}


void irq::request_exception(exception_t exception, exception_handler_t routine) {
	auto& handlers_list = exc_handlers[static_cast<int>(exception)];
	if (handlers_list.push_front(routine) == handlers_list.end()) {
		panic("failed to register exception handler");
	}
}


namespace {


void default_handler(irq::irq_t vector) {
	lib::ccrit << "Unhandled IRQ" << vector << lib::endl;
	panic("Fatal interrupt");
}


irq::irq_return_t exception_dispatcher(irq::irq_t exception,
		irq::stack_ptr_t frame) {
	auto& handlers = exc_handlers[exception];

	if (handlers.empty()) {
		return irq::irq_return_t::none;
	}

	lib::for_each(handlers.begin(), handlers.end(),
		[frame](const irq::exception_handler_t& handler) -> void {
			handler(frame);
	});

	return irq::irq_return_t::handled;
}


irq::irq_return_t interrupts_dispatcher(irq::irq_t vector) {
	auto& handlers = irq_handlers[vector];
	auto used_handler = lib::find_if(handlers.begin(), handlers.end(),
		[vector] (const irq::irq_handler_t &handler) -> bool {
			return handler(vector) == irq::irq_return_t::handled;
	});
	if (used_handler == handlers.end()) {
		return irq::irq_return_t::none;
	}
	return irq::irq_return_t::handled;
}


void irq_dispatcher(irq::irq_t vector, void* frame) {
	irq::irq_return_t status;

	if (vector < static_cast<irq::irq_t>(irq::exception_t::max)) {
		status = exception_dispatcher(vector, frame);
	} else {
		status = interrupts_dispatcher(vector);
	}

	if (status != irq::irq_return_t::handled) {
		default_handler(vector);
	}

	devices::InterruptController::instance()->end_of_interrupt(vector);
}


} // namespace


lib::ostream& irq::operator <<(lib::ostream& out,
		const irq::registers_dump_t& regs) {
	lib::scoped_format_guard format_guard(out);

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
		const irq::execution_info_dump_t& exe) {
	lib::scoped_format_guard format_guard(out);

	out	<< lib::setw(0) << lib::hex
		<< "eflg = " << lib::setw(8) << exe.eflags << lib::setw(0) << ' '
		<< "  cs = " << lib::setw(8) << exe.cs << lib::setw(0) << ' '
		<< " eip = " << lib::setw(8) << exe.eip << lib::setw(0);

	return out;
}


lib::ostream& irq::operator <<(lib::ostream& out,
		const irq::int_frame_error_t& frame) {
	lib::scoped_format_guard format_guard(out);

	out	<< lib::hex
		<< lib::setw(0) << " err = "
		<< lib::setw(8) << frame.error_code
		<< lib::endl
		<< frame.exe << lib::endl
		<< frame.regs;

	return out;
}


lib::ostream& irq::operator <<(lib::ostream& out,
		const irq::int_frame_noerror_t& frame) {
	lib::scoped_format_guard format_guard(out);

	out	<< lib::setw(0) << lib::hex
		<< frame.exe << lib::endl
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

