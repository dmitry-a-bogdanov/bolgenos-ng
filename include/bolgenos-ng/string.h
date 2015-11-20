#ifndef __BOLGENOS_NG__STRING_H__
#define __BOLGENOS_NG__STRING_H__ (1)

#include <bolgenos-ng/int_types.h>

uint32_t uint32_to_string(uint32_t val, char *str, unsigned int base);

int snprintf(char *str, size_t size, const char *format, ...);

uint32_t _strcpy(char *dest, const char *src);

#endif // __BOLGENOS_NG__STRING_H__
