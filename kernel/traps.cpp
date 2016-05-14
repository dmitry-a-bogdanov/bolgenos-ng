#include "traps.hpp"


#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.hpp>
#include <bolgenos-ng/execinfo.hpp>

#include <lib/ostream.hpp>


namespace {

void handle_divide_by_zero(irq::stack_ptr_t frame_pointer);
void handle_debug_exception(irq::stack_ptr_t frame_pointer);
void handle_breakpoint(irq::stack_ptr_t frame_pointer);
void handle_overflow(irq::stack_ptr_t frame_pointer);
void handle_bound_range_exception(irq::stack_ptr_t frame_pointer);
void handle_invalid_opcode(irq::stack_ptr_t frame_pointer);
void handle_dev_not_avail(irq::stack_ptr_t frame_pointer);
void handle_invalid_tss(irq::stack_ptr_t frame_pointer);
void handle_segment_not_present(irq::stack_ptr_t frame_pointer);
void handle_stack_segment_fault(irq::stack_ptr_t frame_pointer);
void handle_gpf(irq::stack_ptr_t frame_pointer);
void handle_page_fault(irq::stack_ptr_t frame_pointer);
void handle_x87_fpe(irq::stack_ptr_t frame_pointer);
void handle_alignment_error(irq::stack_ptr_t frame_pointer);
void handle_mce(irq::stack_ptr_t frame_prointer);
void handle_simd_fpe(irq::stack_ptr_t frame_pointer);
void handle_virtualization_error(irq::stack_ptr_t frame_pointer);
void handle_security_error(irq::stack_ptr_t frame_pointer);


} // namespace


void irq::install_traps() {
	request_exception(irq::exception_t::divide_by_zero,
			handle_divide_by_zero);
	request_exception(irq::exception_t::debug_exception,
			handle_debug_exception);
	request_exception(irq::exception_t::breakpoint,
			handle_breakpoint);
	request_exception(irq::exception_t::overflow_exception,
			handle_overflow);
	request_exception(irq::exception_t::bound_range_exceeded,
			handle_bound_range_exception);
	request_exception(irq::exception_t::invalid_opcode,
			handle_invalid_opcode);
	request_exception(irq::exception_t::device_not_available,
			handle_dev_not_avail);
	request_exception(irq::exception_t::invalid_tss,
			handle_invalid_tss);
	request_exception(irq::exception_t::segment_not_present,
			handle_segment_not_present);
	request_exception(irq::exception_t::stack_segment_fault,
			handle_stack_segment_fault);
	request_exception(irq::exception_t::general_protection_fault,
			handle_gpf);
	request_exception(irq::exception_t::page_fault,
			handle_page_fault);
	request_exception(irq::exception_t::x87_fp_exception,
			handle_x87_fpe);
	request_exception(irq::exception_t::alignment_error,
			handle_alignment_error);
	request_exception(irq::exception_t::machine_check_error,
			handle_mce);
	request_exception(irq::exception_t::simd_fp_exception,
			handle_simd_fpe);
	request_exception(irq::exception_t::virtualization_exception,
			handle_virtualization_error);
	request_exception(irq::exception_t::security_exception,
			handle_security_error);
}


namespace {


void handle_divide_by_zero(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Divide-by-zero has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_debug_exception(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::cnotice << "debug exception has been caught: " << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::cnotice, frame->regs.ebp,
			frame->exe.eip);
}


void handle_breakpoint(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::cnotice << "breakpoint has been caught: " << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::cnotice, frame->regs.ebp,
			frame->exe.eip);
}


void handle_overflow(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Overflow exception has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_bound_range_exception(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Bound range exceeded exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_invalid_opcode(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Invalid opcode exception has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_dev_not_avail(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Device not available exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_invalid_tss(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Invalid TSS exception has been caught!" << lib::endl
			<< *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_segment_not_present(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Segment not present exception has been caught!"
			<< lib::endl << *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_stack_segment_fault(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Stack segment fault exception has been caught!"
			<< lib::endl << *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_gpf(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "General protection fault exception has been caught!"
			<< lib::endl << *frame << lib::endl
			<< "Bad selector index is " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_page_fault(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Page fault exception has been caught!" << lib::endl
			<< *frame << lib::endl
			<< "Error code = " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_x87_fpe(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "x87 floating pointer exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_alignment_error(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);

	lib::ccrit << "Page fault exception has been caught!" << lib::endl
			<< *frame << lib::endl
			<< "Error code = " << frame->error_code
			<< lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_mce(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Machine check error has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_simd_fpe(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "SIMD floating pointer exception has been caught!"
			<< lib::endl << *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_virtualization_error(irq::stack_ptr_t frame_pointer) {
	auto* frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);

	lib::ccrit << "Virtualization error has been caught!" << lib::endl
			<< *frame << lib::endl;
	execinfo::show_backtrace(lib::ccrit, frame->regs.ebp,
			frame->exe.eip);
	panic("Forbidden exception in kernel code!");
}


void handle_security_error(irq::stack_ptr_t frame_pointer) {
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
