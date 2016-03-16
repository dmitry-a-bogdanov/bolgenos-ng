#pragma once

#include "stdtypes.hpp"

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
// typedef void (*irq_handler_t)(irq_t);


/**
* \brief Max IRQ line.
*
* Max IRQ line that is configured and supported. The constant is
*	inetnded to be used for **PIC configuration.
*/
#define MAX_IRQ_NUMBER (0x2f)


/**
* \brief Number of different interrupts.
*
* Number of interrupts that are configured and supported. The constant is
*	inetnded to be used for **PIC configuration.
*/
#define NUMBER_OF_IRQS (MAX_IRQ_NUMBER + 1)

namespace irq {

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

} // namespace irq

/**
* \brief Initialize IRQ subsystem.
*
* Fuction registers generic Interrupt Service Routine and loads
*	Interrupt Descriptor Table.
*/
void setup_interrupts();


/**
* \brief Register interrupt handler function.
*
* Register specified fuction to be called when given IRQ happens.
*
* \param vector IRQ line that will use provided function.
* \param routine Fuction to be called when specified IRQ vector happens.
*/
void register_irq_handler(irq_t vector, irq_handler_t routine);
