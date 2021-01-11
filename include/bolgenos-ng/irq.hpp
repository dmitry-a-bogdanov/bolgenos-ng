#pragma once

#include <cstddef.hpp>
#include <cstdint.hpp>
#include <forward_list.hpp>

#include "asm.hpp"

#include <bolgenos-ng/log.hpp>

namespace lib {
	class ostream;
}


namespace irq {


/// \brief IRQ line type.
///
/// Type for holding IRQ lines. Alias to unsigned one-byte type.
using irq_t = uint8_t;


/// Processor exception type.
enum exception_t: irq_t {
	divide_by_zero			= 0,
	debug_exception			= 1,
	breakpoint			= 3,
	overflow_exception		= 4,
	bound_range_exceeded		= 5,
	invalid_opcode			= 6,
	device_not_available		= 7,
	double_fault,
	// coprocessor segment overrun
	invalid_tss			= 10,
	segment_not_present		= 11,
	stack_segment_fault		= 12,
	general_protection_fault	= 13,
	page_fault			= 14,
	// reserved
	x87_fp_exception		= 16,
	alignment_error			= 17,
	machine_check_error		= 18,
	simd_fp_exception		= 19,
	virtualization_exception	= 20,
	// reserved 21-29
	security_exception		= 30,
	// reserved
	max = 0x20,
};

/// Stack pointer type.
using stack_ptr_t = void *;


class IRQHandler {
public:
	enum class status_t {
		NONE,
		HANDLED,
	};

	virtual ~IRQHandler()
	{
	}


	virtual status_t handle_irq(irq_t vector) = 0;
};


class ExceptionHandler {
public:
	virtual ~ExceptionHandler()
	{
	}

	virtual void handle_exception(stack_ptr_t frame) = 0;
};



/// \brief Last IRQ line.
///
/// Last IRQ line that can be configured configured and supported by xxPIC.
constexpr size_t LAST_LINE = 0xff;


/// \brief Size of IRQ gate.
///
/// IRQ gate must be 8 bytes on x86 arch.
constexpr size_t GATE_SIZE = 8;


/// \brief Total number of IRQ lines.
///
/// Total number of interrupts that are configured and supported by xxPIC.
constexpr size_t NUMBER_OF_LINES = LAST_LINE + 1;



class InterruptsManager {
public:
	void add_handler(irq_t vector, IRQHandler *handler);
	void add_handler(exception_t exception, ExceptionHandler *handler);

	static InterruptsManager *instance();
protected:
	InterruptsManager();

	IRQHandler::status_t dispatch_interrupt(irq_t vector);
	IRQHandler::status_t dispatch_exception(exception_t exception, stack_ptr_t frame_pointer);

	__attribute__((regparm(0),cdecl)) static void handle_irq(irq_t vector, void *frame);
	static bool is_exception(irq_t vector);
private:
	lib::forward_list<IRQHandler *> _irq_handlers[NUMBER_OF_LINES];
	lib::forward_list<ExceptionHandler *> _exceptions_handlers[exception_t::max];
	_irq_aligned_ table_pointer idt_pointer{};

	static InterruptsManager *_instance;
};



/// \brief Enable interrupts.
///
/// Enable interrupts by setting Interrupt Flag for CPU.
inline void enable() {
	lib::cnotice << "enabling interrupts" << lib::endl;
	asm volatile ("sti\n");
}


/// \brief Disable interrupts.
///
/// Disable interrupts by clearing Interrupt Flag for CPU.
///
inline void disable(bool debug = true) {
	if (debug) {
		lib::cnotice << "disabling interrupts" << lib::endl;
	}
	asm volatile ("cli\n");
}


/// \brief Registers dump on stack.
///
/// The structure aliases stack state after calling `pushal`
/// assembler instruction.
struct __attribute__((packed)) registers_dump_t {
	/// Content of EDI register.
	uint32_t	edi;
	/// Content of ESI register.
	uint32_t	esi;
	/// Content of EBP register.
	void*		ebp;
	/// Content of ESP register.
	void*		esp;
	/// Content of EBX register.
	uint32_t	ebx;
	/// Content of EDX register.
	uint32_t	edx;
	/// Content of ECX register.
	uint32_t	ecx;
	/// Content of EAX register.
	uint32_t	eax;
};


/// \brief Execution info.
///
/// The structure aliases data that is stored by CPU when doing context switch
/// to interrupt or exception handler.
struct __attribute__((packed)) execution_info_dump_t {
	/// Instruction pointer.
	void*		eip;
	/// Code segment.
	uint16_t	cs;
	/// Content of EFLAGS register.
	uint32_t	eflags;
};


/// \brief Interrupt frame with error code.
///
/// The structure aliases stack frame after switching to exceptions that stores
/// error code on stack.
struct __attribute__((packed)) int_frame_error_t {
	/// Dump of registers.
	registers_dump_t regs;
	/// Error code.
	uint32_t error_code;
	/// Execution info.
	execution_info_dump_t exe;
};


/// \brief Interrupt frame without error code.
///
/// The structure aliases stack frame after switching to exceptions that doesn't
/// store error code on stack.
struct __attribute__((packed)) int_frame_noerror_t {
	/// Dump of registers.
	registers_dump_t regs;
	/// Execution info.
	execution_info_dump_t exe;
};


// output functions
lib::ostream& operator <<(lib::ostream&, const registers_dump_t&);
lib::ostream& operator <<(lib::ostream&, const execution_info_dump_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_error_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_noerror_t&);


} // namespace irq
