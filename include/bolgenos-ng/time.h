#ifndef __BOLGENOS_NG__TIME_H__
#define __BOLGENOS_NG__TIME_H__ (1)

#include <bolgenos-ng/int_types.h>

extern volatile uint32_t ticks;

void init_timer();

void __sleep(uint32_t ticks_timeout);


#endif // __BOLGENOS_NG__TIME_H__
