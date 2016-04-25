#pragma once

#include <lib/type_traits.hpp>

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
	breakpoint = 3,
	max = 0x20
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
using last_line = lib::integral_constant<size_t, 0x2f>;


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
void register_irq_handler(irq_t vector, irq_handler_t routine);


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


void register_exc_handler(exception_t exception, exc_handler_t handler);


} // namespace irq
