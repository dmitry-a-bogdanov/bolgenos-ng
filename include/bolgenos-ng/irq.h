#ifndef __BOLGENOS_NG__IRQ_H__
#define __BOLGENOS_NG__IRQ_H__

#include <bolgenos-ng/stdtypes.hpp>

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief IRQ line type.
*
* Type for holding IRQ lines. Alias to unsigned one-byte type.
*/
typedef uint8_t irq_t;


/**
* \brief IRQ handler function type.
*
* Type for holding IRQ handler routine. Fuction type accepts IRQ line as
*	parameter and returns nothing.
*/
typedef void (*irq_handler_t)(irq_t);


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


/**
* \brief Enable interrupts.
*
* Enable interrupts by setting Interrupt Flag for CPU.
*/
#define interrupts_enable()						\
	do {								\
		asm ("sti\n");						\
	} while(0)


/**
* \brief Disable interrupts.
*
* Disable interrupts by clearing Interrupt Flag for CPU.
*/
#define interrupts_disable()						\
	do {								\
		asm ("cli\n");						\
	} while(0)


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

#ifdef __cplusplus
}
#endif

#endif // __BOLGENOS_NG__IRQ_H__
