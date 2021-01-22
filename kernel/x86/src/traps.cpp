#include "traps.hpp"


#include <bolgenos-ng/error.h>
#include <bolgenos-ng/irq.hpp>
#include <execinfo.hpp>
#include <log.hpp>


namespace {


class ErrcodeHandler
	: public irq::ExceptionHandler {
public:
	ErrcodeHandler() = default;

	ErrcodeHandler(const ErrcodeHandler&) = delete;
	ErrcodeHandler& operator =(const ErrcodeHandler&) = delete;

	virtual void handle(irq::int_frame_error_t *frame_pointer) = 0;

	void handle_exception(irq::stack_ptr_t frame_pointer) override
	{
		auto* frame = static_cast<irq::int_frame_error_t *>(frame_pointer);
		this->handle(frame);
	}
};


class NoErrcodeHandler
	: public irq::ExceptionHandler {
public:
	NoErrcodeHandler() = default;

	NoErrcodeHandler(const NoErrcodeHandler&) = delete;
	NoErrcodeHandler& operator =(const NoErrcodeHandler&) = delete;

	virtual void handle(irq::int_frame_noerror_t *frame_pointer) = 0;

	void handle_exception(irq::stack_ptr_t frame_pointer) override
	{
		auto frame = static_cast<irq::int_frame_noerror_t *>(frame_pointer);
		this->handle(frame);
	}
};


class PrintingErrcodeHandler
	: public ErrcodeHandler {
public:
	PrintingErrcodeHandler() = delete;

	PrintingErrcodeHandler(const PrintingErrcodeHandler&) = delete;
	PrintingErrcodeHandler& operator =(const PrintingErrcodeHandler&) = delete;

	explicit PrintingErrcodeHandler(const char *message)
		: ErrcodeHandler(), _message(message)
	{
	}

	void handle(irq::int_frame_error_t *frame_pointer) override
	{
		lib::ccrit << _message << lib::endl
				<< *frame_pointer << lib::endl
				<< "error code is " << frame_pointer->error_code << lib::endl;
		execinfo::show_backtrace(lib::ccrit, frame_pointer->regs.ebp,
				frame_pointer->exe.eip);
		panic("Forbidden exception in kernel code!");
	}

private:
	const char *_message;
};


class PrintingNoErrcodeHandler
	: public NoErrcodeHandler {
public:
	PrintingNoErrcodeHandler() = delete;

	PrintingNoErrcodeHandler(const PrintingNoErrcodeHandler&) = delete;
	PrintingNoErrcodeHandler& operator =(const PrintingNoErrcodeHandler&) = delete;

	explicit PrintingNoErrcodeHandler(const char *message)
		: NoErrcodeHandler(), _message(message)
	{
	}

	void handle(irq::int_frame_noerror_t *frame_pointer) override
	{
		lib::ccrit << _message << lib::endl << *frame_pointer << lib::endl;
		execinfo::show_backtrace(lib::ccrit, frame_pointer->regs.ebp,
				frame_pointer->exe.eip);
		panic("Forbidden exception in kernel mode!");
	}
private:
	const char *_message;
};


} // namespace


void irq::install_traps() {
	auto irq_manager = irq::InterruptsManager::instance();
	irq_manager->add_handler(irq::exception_t::divide_by_zero,
			new PrintingNoErrcodeHandler("Division by zero"));
	irq_manager->add_handler(irq::exception_t::debug_exception,
			new PrintingNoErrcodeHandler("Debug exception"));
	irq_manager->add_handler(irq::exception_t::breakpoint,
			new PrintingNoErrcodeHandler("Breakpoint"));
	irq_manager->add_handler(irq::exception_t::overflow_exception,
			new PrintingNoErrcodeHandler("Overflow"));
	irq_manager->add_handler(irq::exception_t::bound_range_exceeded,
			new PrintingNoErrcodeHandler("Bound range exceeded"));
	irq_manager->add_handler(irq::exception_t::invalid_opcode,
			new PrintingNoErrcodeHandler("Invalid opcode"));
	irq_manager->add_handler(irq::exception_t::device_not_available,
			new PrintingNoErrcodeHandler("Device not available"));


	irq_manager->add_handler(irq::exception_t::invalid_tss,
			new PrintingErrcodeHandler("Invalid TSS"));
	irq_manager->add_handler(irq::exception_t::segment_not_present,
			new PrintingErrcodeHandler("MemorySegmentDescriptor not present"));
	irq_manager->add_handler(irq::exception_t::stack_segment_fault,
			new PrintingErrcodeHandler("Stack segment fault"));
	irq_manager->add_handler(irq::exception_t::general_protection_fault,
			new PrintingErrcodeHandler("General protection fault"));
	irq_manager->add_handler(irq::exception_t::page_fault,
			new PrintingErrcodeHandler("Page fault"));


	irq_manager->add_handler(irq::exception_t::x87_fp_exception,
			new PrintingNoErrcodeHandler("x87 FPU exception"));


	irq_manager->add_handler(irq::exception_t::alignment_error,
			new PrintingErrcodeHandler("Alignment error"));


	irq_manager->add_handler(irq::exception_t::machine_check_error,
			new PrintingNoErrcodeHandler("Machine check error"));
	irq_manager->add_handler(irq::exception_t::simd_fp_exception,
			new PrintingNoErrcodeHandler("SIMD exception"));
	irq_manager->add_handler(irq::exception_t::virtualization_exception,
			new PrintingNoErrcodeHandler("Virtualization error"));


	irq_manager->add_handler(irq::exception_t::security_exception,
			new PrintingErrcodeHandler("Security exception"));
}

