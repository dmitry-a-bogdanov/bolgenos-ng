#pragma once

#include "compiler.h"

#include "stdtypes.hpp"


/**
* \brief Universal pointer type.
*
* Universal pointer type aliased to pointer to void.
*/
typedef void *ptr_t;


/// \brief Type of the null pointer constant nullptr.
///
/// According to standard this type can only take one value: nullptr,
/// which when converted to a pointer type takes the proper null pointer value.
/// But I'm not sure that it's correct for freestanding system. Let's check:)
using nullptr_t = decltype(nullptr);


/// \brief Helper for applying bitmasks.
///
/// Overloading for nullptr.
///
/// \return Zero casted to ValueType.
template<typename ValueType>
constexpr ValueType bitmask(nullptr_t) {
	return ValueType(0);
};


/// \brief Helper for applying bit masks.
///
/// Macro returns value that is calculated from given value that is shifted
/// right to given offset bytes and OR-ed with given mask.
///
/// \tparam ValueType type of value for function.
/// \tparam OffsetType type of offset of function.
/// \tparam MaskType type of mask for function.
/// \param value Value to be transformed by bit mask.
/// \param offset Offset for right-shifting of value.
/// \param mask Bitmask that is to be applied to shifted value.
/// \return shifted value with applied mask casted to ValueType.
template<typename ValueType, typename OffsetType, typename MaskType>
constexpr ValueType bitmask(ValueType value, OffsetType offset, MaskType mask) {
	return (((value) >> (offset)) & (mask));
};


/**
* \brief Internal write 8-bit value to address.
*
* Write 8-bit value from source to destination. The fuction can be used for
*	writing to memory that is used for hardware I/O in destiation address
*	since it forcing cast to volatile value. Function is to be used only
*	in write_* function group.
* \param dest Destination address.
* \param src Source address.
*/
static inline void __write_8(volatile char *dest, char *src) {
	*dest = *src;
}


/**
* \brief Wrapper for __write_8 function.
*
* Fuction accepts that does exactly that the same as __write_8, but doesn't
*	requires pointer to volatile value as destination address.
* \param dest Destination address.
* \param src Source address.
*/
static inline void write_8(char *dest, char *src) {
	__write_8((volatile char *) dest, src);
}


/**
* \brief Write 16-bit value.
*
* Copy 16-bit value from source to destination. Can be used for I/O operations.
* \param dest Destination address.
* \param src Source address.
*/
static inline void write_16(char *dest, char *src) {
	write_8(dest, src);
	write_8(dest + 1, src + 1);
}


/**
* \brief Analog for POSIX memset.
*
* The fuction does the same as memset POSIX with small difference. This
*	implementation returns nothing.
* \param mem Memory to be set.
* \param val Value that will be used for setting memory. Only least significant
*	byte will be used.
* \param size Size of memory to be set in bytes.
*/
void memset(void *mem, char val, size_t size);

/**
* \brief Memset that uses 16-bit chunk for setting memory.
*
* \param mem Memory to be set.
* \param chunk Address of 16-bit chunk that will be used for setting memory.
* \param size Size of memory to be set in 2-byte units.
*/
static inline void memset_16(char* mem, char* chunk, size_t size) {
	for(size_t pos = 0; pos != size; ++pos, mem += 2) {
		write_16(mem, chunk);
	}
}


/**
* \brief Analog for POSIX memcpy.
*
* The fuction does the same as memcpy POSIX with small difference. This
*	implementation returns nothing.
* \param dest Destinatination address for memory copying.
* \param src Source address for memory copying.
* \param size Size of memory to be copied in bytes.
*/
static inline void memcpy(void *dest, const void *src, size_t size) {
	char *__dest = (char *) dest;
	char *__src = (char* ) src;
	for (size_t pos = 0; pos != size; ++pos) {
		__dest[pos] = __src[pos];
	}
}
