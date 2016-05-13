#pragma once

#include <lib/type_traits.hpp>

#include "asm.h"
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


using stack_ptr_t = void *;


enum class irq_return_t {
	none,
	handled,
};


/// \brief IRQ handler function type.
///
/// Type for holding IRQ handler routine. Function type accepts IRQ line as
/// parameter and returns nothing.
using irq_handler_t = irq_return_t (*)(irq_t);


/// \brief Last IRQ line.
///
/// Last IRQ line that can be configured configured and supported by xxPIC.
using last_line = lib::integral_constant<size_t, 0xff>;

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


struct __attribute__((packed)) registers_dump_t {
	uint32_t	edi;
	uint32_t	esi;
	void*		ebp;
	void*		esp;
	uint32_t	ebx;
	uint32_t	edx;
	uint32_t	ecx;
	uint32_t	eax;
};


struct __attribute__((packed)) execution_info_dump_t {
	void*		eip;
	uint16_t	cs;
	uint32_t	eflags;
};


struct __attribute__((packed)) int_frame_error_t {
	registers_dump_t regs;
	uint32_t error_code;
	execution_info_dump_t exe;
};


struct __attribute__((packed)) int_frame_noerror_t {
	registers_dump_t regs;
	execution_info_dump_t exe;
};


lib::ostream& operator <<(lib::ostream&, const registers_dump_t&);
lib::ostream& operator <<(lib::ostream&, const execution_info_dump_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_error_t&);
lib::ostream& operator <<(lib::ostream&, const int_frame_noerror_t&);


using exc_handler_t = void (*)(void *frame);


void request_exception(exception_t exception, exc_handler_t handler);




extern "C" void irq_dispatcher(irq::irq_t vector, void* frame);


} // namespace irq
