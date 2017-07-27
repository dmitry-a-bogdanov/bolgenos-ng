#pragma once

namespace std {


template<bool Cond, class T = void>
struct enable_if {};


template<class T>
struct enable_if<true, T>
{
	using type = T;
};


template<bool Cond, class T = void>
using enable_if_t = typename enable_if<Cond, T>::type;


} // namespace std
