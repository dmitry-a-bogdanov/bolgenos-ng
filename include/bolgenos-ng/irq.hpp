#pragma once

#include <lib/type_traits.hpp>

#include "asm.hpp"
#include "mmu.hpp"
#include "mem_utils.hpp"
#include "stdtypes.hpp"


namespace lib {
	class ostream;
}


namespace irq {


/// \brief IRQ line type.
///
/// Type for holding IRQ lines. Alias to unsigned one-byte type.
using irq_t = uint8_t;


/// Processor exception type.
enum class exception_t: irq_t {
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


/// IRQ handling status.
enum class irq_return_t {
	/// IRQ was not handled by the routine.
	none,
	/// IRQ was handled by the routine.
	handled,
};


/// \brief IRQ handler function pointer type.
///
/// Type for holding IRQ handler routine. Function type accepts IRQ line as
/// parameter and returns irq_return_t that tells was this IRQ handled by
/// this function or not.
using irq_handler_t = irq_return_t (*)(irq_t);


/// \brief Type of exception handler function pointer.
///
/// Type for holding exception handler routine. Function accepts pointer
/// to stack frame of exception.
using exception_handler_t = void (*)(stack_ptr_t frame);

/// \brief Last IRQ line.
///
/// Last IRQ line that can be configured configured and supported by xxPIC.
using last_line = lib::integral_constant<size_t, 0xff>;


/// \brief Size of IRQ gate.
///
/// IRQ gate must be 8 bytes on x86 arch.
using gate_size = lib::integral_constant<size_t, 8>;


/// \brief Total number of IRQ lines.
///
/// Total number of interrupts that are configured and supported by xxPIC.
using lines_number = lib::integral_constant<size_t, last_line::value + 1>;


/// \brief Enable interrupts.
///
/// Enable interrupts by setting Interrupt Flag for CPU.
inline void enable() {
	asm volatile ("sti\n");
}


/// \brief Disable interrupts.
///
/// Disable interrupts by clearing Interrupt Flag for CPU.
///
inline void disable() {
	asm volatile ("cli\n");
}


/// \brief Initialize IRQ subsystem.
///
/// Function registers generic Interrupt Service Routine and loads
/// Interrupt Descriptor Table.
void init();


/// \brief Register interrupt handler function.
///
/// Register specified function to be called when given IRQ happens.
///
/// \param vector IRQ line that will use provided function.
/// \param routine Function to be called when specified IRQ vector happens.
void request_irq(irq_t vector, irq_handler_t routine);


/// \brief Register exception handler function.
///
/// Register specified function to be called when given exception happens.
///
/// \param exception Exception number that will use provided function.
/// \param handler Function to be called when specified exception happens.
void request_exception(exception_t exception, exception_handler_t handler);


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
	x86::registers_dump_t regs;
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
	x86::registers_dump_t regs;
	/// Execution info.
	execution_info_dump_t exe;
};


// output functions
lib::ostream& operator <<(lib::ostream&, const execution_info_dump_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_error_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_noerror_t&);


} // namespace irq
