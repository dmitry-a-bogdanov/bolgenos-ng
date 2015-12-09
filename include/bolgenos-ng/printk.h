#ifndef __BOLGENOS_NG__PRINTK_H__
#define __BOLGENOS_NG__PRINTK_H__ (1)

#include <stdarg.h>

int printk(const char *fmt, ...);

int vprintk(const char *fmt, va_list args);

#endif // __BOLGENOS_NG__PRINTK_H__
