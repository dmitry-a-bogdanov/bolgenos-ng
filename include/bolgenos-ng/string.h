#ifndef __BOLGENOS_NG__STRING_H__
#define __BOLGENOS_NG__STRING_H__

#include <stdarg.h>

#include <bolgenos-ng/stdtypes.hpp>


#ifdef __cplusplus
extern "C" {
#endif


/**
* \brief Calculate length of specified string.
*
* The fuction works like a strlen from POSIX.
* \param str Pointer to string.
* \return Length of string.
*/
size_t strlen(const char *str);


/**
* \brief Formatted output to string.
*
* The fuction works like a POSIX-version but not all formats are supported.
* The following formats are supported:
*	- li
*	- lu
*	- s
* \param str Pointer to output string.
* \param size Size of maximal possible output including null termination byte.
* \param format Format string.
* \param ... Additinal arguments for format specification.
* \return Number of written symbol excluding null termination byte.
*/
int snprintf(char *str, size_t size, const char *format, ...);


/**
* \brief Formatted output to string. Version for va_list.
*
* The fuction works like a POSIX-version but not all formats are supported.
* The following formats are supported:
*	- li
*	- lu
*	- s
* \param str Pointer to output string.
* \param size Size of maximal possible output including null termination byte.
* \param format Format string.
* \param args Pointer to container of variadic arguments for format
*	specification.
* \return Number of written symbol excluding null termination byte.
*/
int vsnprintf(char *str, size_t size, const char *format, va_list args);


/**
* \brief POSIX-like strcpy.
*
* Copy string.
* \param dest Pointer to destination.
* \param src Pointer to source.
* \return Pointer to destination.
*/
char *strcpy(char *dest, const char *src);


/**
* \brief POSIX-like strcpy.
*
* Copy no more than n symbols of string from source to destination.
* \param dest Pointer to destination.
* \param src Pointer to source.
* \param n Upper bound for number of copied symbols.
* \return Pointer to destination.
*/
char *strncpy(char *dest, const char *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif // __BOLGENOS_NG__STRING_H__
