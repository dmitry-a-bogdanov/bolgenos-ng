#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"

namespace lib {


template<class T>
constexpr T&& forward(remove_reference_t<T>& t) noexcept
{
	return static_cast<T&&>(t);
}

template<class T>
constexpr T&& forward(remove_reference_t<T>&& t) noexcept
{
	return static_cast<T&&>(t);
}


template<class T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept
{
	return static_cast<remove_reference_t<T>&&>(t);
}


template<class T>
void swap(T& a, T& b)
{
	T tmp = move(b);
	b = move(a);
	a = move(tmp);
}


template<class T, size_t N>
void swap(T (&a)[N], T (&b)[N])
{
	for (size_t index = 0; index != N; ++index)
	{
		swap(a[index], b[index]);
	}
}

template<typename T, T... I>
struct integer_sequence
{
	typedef T value_type;
	static constexpr size_t size() noexcept { return sizeof...(I); }
};

template<typename T, T N>
using make_integer_sequence = integer_sequence<T, __integer_pack(N)...>;

template<size_t... _Idx>
using index_sequence = integer_sequence<size_t, _Idx...>;

template<size_t _Num>
using make_index_sequence = make_integer_sequence<size_t, _Num>;


} // namespace lib
