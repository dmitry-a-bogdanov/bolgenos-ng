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
