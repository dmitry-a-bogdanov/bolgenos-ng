#include <bolgenos-ng/traps.hpp>


#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/execinfo.hpp>

#include <lib/ostream.hpp>


namespace {

void do_divide_by_zero(void *frame_pointer);	// 0x0, #DE
void do_debug(void *frame_pointer);		// 0x1, #DB
void do_breakpoint(void* frame_pointer);	// 0x3, #BP


} // namespace


void irq::install_traps() {
	register_exc_handler(irq::exception_t::divide_by_zero,
			do_divide_by_zero);
	register_exc_handler(irq::exception_t::debug_exception,
			do_debug);
	register_exc_handler(irq::exception_t::breakpoint,
			do_breakpoint);
}


namespace {


void do_divide_by_zero(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Divide-by-zero has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_debug(void* frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::cnotice << "debug exception has been caught: " << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::cnotice, frame->regs.ebp,
			frame->exe.eip);
}


void do_breakpoint(void* frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::cnotice << "breakpoint has been caught: " << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::cnotice, frame->regs.ebp,
			frame->exe.eip);
}


} // namespace
