#include <bolgenos-ng/traps.hpp>


#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/execinfo.hpp>

#include <lib/ostream.hpp>


namespace {


void do_breakpoint(void* frame_pointer);


} // namespace


void irq::install_traps() {
	register_exc_handler(irq::exception_t::breakpoint, do_breakpoint);
}


namespace {


void do_breakpoint(void* frame_pointer) {
	lib::cnotice << "breakpoint has been caught: " << lib::endl;

	irq::int_frame_noerror_t* frame
		= static_cast<irq::int_frame_noerror_t *>(frame_pointer);
	lib::cnotice << *frame << lib::endl;
	execinfo::show_backtrace(lib::cnotice, frame->regs.ebp,
			frame->exe.eip);
}


} // namespace
