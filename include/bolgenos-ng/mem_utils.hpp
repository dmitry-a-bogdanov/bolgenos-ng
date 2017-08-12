#pragma once

#include <cstddef>
#include <type_traits>


template<typename ValueType, typename MaskType>
constexpr std::remove_reference_t<ValueType>
bitmask(const ValueType value,
		std::size_t offset,
		const MaskType mask)
{
	using value_type = std::remove_reference_t<ValueType>;
	using mask_type = std::remove_reference_t<MaskType>;
	static_assert(std::is_unsigned_v<value_type>, "ValueType must be unsigned");
	static_assert(std::is_unsigned_v<mask_type>, "MaskType must be unsigned");
	return (value >> offset) & value_type{ mask };
}
