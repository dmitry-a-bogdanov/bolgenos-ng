#pragma once

#include "cstddef.hpp"
#include "type_traits.hpp"

namespace lib {


template<class T>
constexpr T&& forward(typename remove_reference<T>::type& t) noexcept
{
	return t;
}

template<class T>
constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept
{
	return t;
}


template<class T>
constexpr typename remove_reference<T>::type&& move(T&& t) noexcept
{
	return static_cast<typename remove_reference<T>::type&&>(t);
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

} // namespace lib
