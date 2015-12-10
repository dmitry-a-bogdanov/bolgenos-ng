#ifndef __BOLGENOS_NG__STRING_H__
#define __BOLGENOS_NG__STRING_H__ (1)

#include <stdarg.h>

#include <bolgenos-ng/int_types.h>

size_t strlen(const char *str);

int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list args);

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);

#endif // __BOLGENOS_NG__STRING_H__
