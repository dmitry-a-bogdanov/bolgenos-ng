#ifndef __BOLGENOS_NG__STRING_H__
#define __BOLGENOS_NG__STRING_H__ (1)

#include <bolgenos-ng/int_types.h>

uint32_t uint32_to_string(uint32_t val, char *str, unsigned int base);
uint32_t uint64_to_string(uint64_t val, char *str, unsigned int base);

#endif // __BOLGENOS_NG__STRING_H__
