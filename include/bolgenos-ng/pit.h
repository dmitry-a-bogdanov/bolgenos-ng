#ifndef __BOLGENOS_NG__PIT_H__
#define __BOLGENOS_NG__PIT_H__

#include <bolgenos-ng/int_types.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_pit();

uint32_t ms_to_ticks(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // __BOLGENOS_NG__PIT_H__
