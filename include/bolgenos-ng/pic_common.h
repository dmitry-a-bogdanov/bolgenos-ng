#ifndef __BOLGENOS_NG__PIC_COMMON__
#define __BOLGENOS_NG__PIC_COMMON__ (1)

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/irq.h>

struct pic_device {
	void (*setup)();
	void (*end_of_interrupt)(irq_t irq);
	int min_irq_vector;
};

extern struct pic_device *system_pic;

#define min_pic_irq		(system_pic->min_irq_vector)
#define end_of_irq(vec)		system_pic->end_of_interrupt(vec)

#endif // __BOLGENOS_NG__PIC_COMMON__
