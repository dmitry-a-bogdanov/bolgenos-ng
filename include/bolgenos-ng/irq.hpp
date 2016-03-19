#pragma once

#include <lib/type_traits.hpp>

#include "stdtypes.hpp"


namespace irq {


/// \brief IRQ line type.
///
/// Type for holding IRQ lines. Alias to unsigned one-byte type.
using irq_t = uint8_t;


/// \brief IRQ handler function type.
///
/// Type for holding IRQ handler routine. Function type accepts IRQ line as
/// parameter and returns nothing.
using irq_handler_t = void (*)(irq_t);


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
void register_handler(irq_t vector, irq_handler_t routine);


} // namespace irq
