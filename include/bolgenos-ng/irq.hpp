#pragma once

#include "stdtypes.hpp"
#include "type_traits.hpp"


namespace irq {

/**
* \brief IRQ line type.
*
* Type for holding IRQ lines. Alias to unsigned one-byte type.
*/
// typedef uint8_t irq_t;
using irq_t = uint8_t;


/**
* \brief IRQ handler function type.
*
* Type for holding IRQ handler routine. Fuction type accepts IRQ line as
*	parameter and returns nothing.
*/
using irq_handler_t = void (*)(irq_t);


/**
* \brief Max IRQ line.
*
* Max IRQ line that is configured and supported. The constant is
*	inetnded to be used for **PIC configuration.
*/
using max_irq_number = integral_constant<size_t, 0x2f>;


/**
* \brief Number of different interrupts.
*
* Number of interrupts that are configured and supported. The constant is
*	inetnded to be used for **PIC configuration.
*/
using number_of_irqs = integral_constant<size_t, max_irq_number::value>;


/**
* \brief Enable interrupts.
*
* Enable interrupts by setting Interrupt Flag for CPU.
*/
inline void enable() {
	asm volatile ("sti\n");
}


/**
* \brief Disable interrupts.
*
* Disable interrupts by clearing Interrupt Flag for CPU.
*/
inline void disable() {
	asm volatile ("cli\n");
}


/**
* \brief Initialize IRQ subsystem.
*
* Fuction registers generic Interrupt Service Routine and loads
*	Interrupt Descriptor Table.
*/
void init();


/**
* \brief Register interrupt handler function.
*
* Register specified fuction to be called when given IRQ happens.
*
* \param vector IRQ line that will use provided function.
* \param routine Fuction to be called when specified IRQ vector happens.
*/
void register_handler(irq_t vector, irq_handler_t routine);


} // namespace irq
