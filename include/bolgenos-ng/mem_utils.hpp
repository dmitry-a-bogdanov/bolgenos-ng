#pragma once

#include <cstddef.hpp>

#include <type_traits.hpp>

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
constexpr ValueType shiftmask(ValueType value, OffsetType offset, MaskType mask) {
	return (((value) >> (offset)) & (mask));
};


/**
* \brief Align specified value up.
*
* Align given value up to specified boundary.
*
* \tparam Boundary Alignment boundary.
* \tparam ValueType Type of value that is to be aligned.
* \param value Value to be aligned.
* \return Aligned value.
*/
template<size_t Boundary, typename ValueType>
constexpr inline ValueType align_up(ValueType value) {
	constexpr size_t alignment_mask = Boundary - 1;
	auto _val = reinterpret_cast<size_t>(value);
	if (_val & alignment_mask) {
		_val = align_down<Boundary>(_val) + Boundary;
	}
	return reinterpret_cast<ValueType>(_val);
}



template<lib::uintmax_t Boundary, typename T>
constexpr T align_down(T value)
{
	if constexpr (lib::is_pointer_v<T>)
	{
		return align_down<Boundary, lib::uintptr_t>(value);
	}
	else
	{
		using Type = lib::make_unsigned_t<T>;
		Type mask = ~(static_cast<Type>(Boundary) - 1);
		Type retval = static_cast<Type>(value);
		retval &= mask;
		return static_cast<T>(retval);
	}
}


template<size_t Boundary, typename ValueType>
constexpr inline bool is_aligned_at_least(ValueType value) {
	return align_up<Boundary>(value) == value;
}


