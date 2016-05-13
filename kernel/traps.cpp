#include <bolgenos-ng/traps.hpp>


#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/execinfo.hpp>

#include <lib/ostream.hpp>


namespace {

void do_divide_by_zero(void *frame_pointer);	// 0x0,  #DE
void do_debug(void *frame_pointer);		// 0x1,  #DB
void do_breakpoint(void* frame_pointer);	// 0x3,  #BP
void do_overflow(void *frame_pointer);		// 0x4,  #OF
void do_bound_range(void *frame_pointer);	// 0x5,  #BR
void do_invalid_opcode(void *frame_pointer);	// 0x6,  #UD
void do_dev_not_available(void *frame_pointer);	// 0x7,  #NM
void do_invalid_tss(void *frame_pointer);	// 0xA,  #TS
void do_no_segment(void *frame_pointer);	// 0xB,  #NP
void do_ss_fault(void *frame_pointer);		// 0xC,  #SS
void do_gpf(void *frame_pointer);		// 0xD,  #GP
void do_page_fault(void *frame_pointer);	// 0xE,  #PF
void do_x87_fpe(void *frame_pointer);		// 0x10, #MF
void do_ace(void *frame_pointer);		// 0x11, #AC
void do_mce(void *frame_prointer);		// 0x12, #MC
void do_simd_fpe(void *frame_pointer);		// 0x13, #XM/#XF
void do_virt_error(void *frame_pointer);	// 0x14, #VE
void do_security_error(void *frame_pointer);	// 0x1E, #SX


} // namespace


void irq::install_traps() {
	register_exc_handler(irq::exception_t::divide_by_zero,
			do_divide_by_zero);
	register_exc_handler(irq::exception_t::debug_exception,
			do_debug);
	register_exc_handler(irq::exception_t::breakpoint,
			do_breakpoint);
	register_exc_handler(irq::exception_t::overflow_exception,
			do_overflow);
	register_exc_handler(irq::exception_t::bound_range_exceeded,
			do_bound_range);
	register_exc_handler(irq::exception_t::invalid_opcode,
			do_invalid_opcode);
	register_exc_handler(irq::exception_t::device_not_available,
			do_dev_not_available);
	register_exc_handler(irq::exception_t::invalid_tss,
			do_invalid_tss);
	register_exc_handler(irq::exception_t::segment_not_present,
			do_no_segment);
	register_exc_handler(irq::exception_t::stack_segment_fault,
			do_ss_fault);
	register_exc_handler(irq::exception_t::general_protection_fault,
			do_gpf);
	register_exc_handler(irq::exception_t::page_fault,
			do_page_fault);
	register_exc_handler(irq::exception_t::x87_fp_exception,
			do_x87_fpe);
	register_exc_handler(irq::exception_t::alignment_error,
			do_ace);
	register_exc_handler(irq::exception_t::machine_check_error,
			do_mce);
	register_exc_handler(irq::exception_t::simd_fp_exception,
			do_simd_fpe);
	register_exc_handler(irq::exception_t::virtualization_exception,
			do_virt_error);
	register_exc_handler(irq::exception_t::security_exception,
			do_security_error);
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


void do_overflow(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Overflow exception has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_bound_range(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Bound range exceeded exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_invalid_opcode(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Invalid opcode exception has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_dev_not_available(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Device not available exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_invalid_tss(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Invalid TSS exception has been caught!" << lib::endl
			<< *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_no_segment(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Segment not present exception has been caught!"
			<< lib::endl << *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_ss_fault(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Stack segment fault exception has been caught!"
			<< lib::endl << *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_gpf(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "General protection fault exception has been caught!"
			<< lib::endl << *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_page_fault(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Page fault exception has been caught!" << lib::endl
			<< *frame << lib::endl
			<< "Error code = " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_x87_fpe(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "x87 floating pointer exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_ace(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Page fault exception has been caught!" << lib::endl
			<< *frame << lib::endl
			<< "Error code = " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_mce(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Machine check error has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_simd_fpe(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "SIMD floating pointer exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_virt_error(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Virtualization error has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void do_security_error(void *frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Page fault exception has been caught!" << lib::endl
			<< *frame << lib::endl
			<< "Error code = " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


} // namespace
