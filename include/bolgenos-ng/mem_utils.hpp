#pragma once

#include <cstddef>


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
}
