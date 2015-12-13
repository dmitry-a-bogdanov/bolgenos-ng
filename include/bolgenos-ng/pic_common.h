#ifndef __BOLGENOS_NG__PIC_COMMON__
#define __BOLGENOS_NG__PIC_COMMON__

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/irq.h>


/**
* \brief Generic PIC device.
*
* Structure that describes generic PIC device. It can be used for
*	handling PIC device line PIC 8259, APIC or X2APIC.
*/
struct pic_device {
	void (*setup)(); /*!< Function that initilizes PIC device. */

	void (*end_of_interrupt)(irq_t irq); /*!< Function that send
*			"End of interrupt" message to generic PIC device for
*			given IRQ line. */

	int min_irq_vector; /*!< The first IRQ line number that can be received
*			from the PIC chip. */

};


/**
* \brief Pointer to PIC device structure that is used on the system.
*/
extern struct pic_device *system_pic;


/**
* \brief The first IRQ line number that can be received from the PIC chip.
*
* Alias to system_pic->min_irq_vector.
*
*/
#define min_pic_irq		(system_pic->min_irq_vector)


/**
* \brief Send "End of interrupt" message to system PIC device.
*
* Alias to system_pic->end_of_interrupt
*/
#define end_of_irq(vec)		system_pic->end_of_interrupt(vec)

#endif // __BOLGENOS_NG__PIC_COMMON__
