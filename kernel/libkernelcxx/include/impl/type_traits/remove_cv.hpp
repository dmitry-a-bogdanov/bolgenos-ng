#pragma once

namespace lib {


template<class T>
struct remove_const
{
	using type = T;
};


template<class T>
struct remove_const<const T>
{
	using type = T;
};


template<class T>
using remove_const_t = typename remove_const<T>::type;



template<class T>
struct remove_volatile
{
	using type = T;
};


template<class T>
struct remove_volatile<volatile T>
{
	using type = T;
};


template<class T>
using remove_volatile_t = typename remove_volatile<T>::type;


template<class T>
struct remove_cv
{
	using type = remove_const_t<remove_volatile_t<T>>;
};


template<class T>
using remove_cv_t = typename remove_cv<T>::type;


} // namespace lib
