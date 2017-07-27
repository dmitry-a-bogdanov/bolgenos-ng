#pragma once

#include "enable_if.hpp"
#include "is_unsigned.hpp"

namespace std {


template<typename T>
struct make_unsigned {
	using type = typename enable_if<is_unsigned<T>::value, T>::type;
};


template<>
struct make_unsigned<char> {
	using type = unsigned char;
};


template<>
struct make_unsigned<short> {
	using type = unsigned short;
};


template<>
struct make_unsigned<int> {
	using type = unsigned int;
};


template<>
struct make_unsigned<long> {
	using type = unsigned long;
};


template<>
struct make_unsigned<long long> {
	using type = unsigned long long;
};


template<class T>
using make_unsigned_t = typename make_unsigned<T>::type;

} // namespace std
