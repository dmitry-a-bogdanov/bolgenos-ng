#pragma once

#if defined(__cplusplus)
#include <cstddef.hpp>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
* \brief Calculate length of specified string.
*
* The function works like a strlen from POSIX.
* \param str Pointer to string.
* \return Length of string.
*/
size_t strlen(const char *str);


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

void *memset(void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif
