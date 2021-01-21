#pragma once

namespace lib {

template<class T>
struct add_const
{
	using type = const T;
};

template<class T>
using add_const_t = typename add_const<T>::type;



template<class T>
struct add_lvalue_reference
{
	using type = T&;
};

template<class T>
struct add_lvalue_reference<T&&>
{
	using type = T&;
};

template<class T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;



template<class T>
struct add_rvalue_reference
{
	using type = T&&;
};

template<class T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;



template<class T>
struct remove_reference
{
	using type = T;
};

template<class T>
struct remove_reference<T&>
{
	using type = T;
};

template<class T>
struct remove_reference<T&&>
{
	using type = T;
};

template<class T>
using remove_reference_t = typename remove_reference<T>::type;

}
