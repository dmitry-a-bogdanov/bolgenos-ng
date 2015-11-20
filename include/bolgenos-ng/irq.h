#ifndef __BOLGENOS_NG__IRQ_H__
#define __BOLGENOS_NG__IRQ_H__ (1)

#include <bolgenos-ng/int_types.h>

typedef uint8_t irq_t;

#define MAX_IRQ_NUMBER (0x2f)
#define NUMBER_OF_IRQS (MAX_IRQ_NUMBER + 1)

#define interrupts_enable()						\
	do {								\
		asm ("sti\n");						\
	} while(0)

#define interrupts_disable()						\
	do {								\
		asm ("cli\n");						\
	} while(0)

void setup_interrupts();

#endif // __BOLGENOS_NG__IRQ_H__
