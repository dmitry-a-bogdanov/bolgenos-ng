#pragma once

#include <type_traits>

namespace basalt
{


template<class T>
constexpr std::enable_if_t<std::is_enum_v<T>, std::underlying_type_t<T>> to_integral(T v)
{
	return static_cast<std::underlying_type_t<T>>(v);
}


}
