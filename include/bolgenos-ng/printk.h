#pragma once

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
