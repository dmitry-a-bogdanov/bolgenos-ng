#ifndef __BOLGENOS_NG__PRINTK_H__
#define __BOLGENOS_NG__PRINTK_H__

#include <stdarg.h>


/**
* \brief Print formatted message.
*
* Print specified formatted message to default kernel's output.
* \param fmt Formatting string.
* \param ... Additional arguments specified by formatting string.
* \return Number of printed symbols.
*/
int printk(const char *fmt, ...);


/**
* \brief Print formatted message with va_list argument.
*
* Print specified formatted message to default kernel's output.
* \param fmt Formatting string.
* \param args Container with additional arguments specified by
*	formatting string.
* \return Number of printed symbols.
*/
int vprintk(const char *fmt, va_list args);

#endif // __BOLGENOS_NG__PRINTK_H__
