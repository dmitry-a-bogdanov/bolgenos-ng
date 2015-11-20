#ifndef __BOLGENOS_NG__PIC_COMMON__
#define __BOLGENOS_NG__PIC_COMMON__ (1)

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/irq.h>

struct pic_device {
	void (*setup)();
	void (*end_of_interrupt)(irq_t irq);
};

extern struct pic_device *system_pic;

#endif // __BOLGENOS_NG__PIC_COMMON__
