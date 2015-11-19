#ifndef __BOLGENOS_NG__APIC_H__
#define __BOLGENOS_NG__APIC_H__ (1)

#include <bolgenos-ng/int_types.h>

void pic_8259_setup();
void pic_8259_send_end_of_interrput(uint8_t irq);

#endif // __BOLGENOS_NG__APIC_H__
