#pragma once

#include <cstddef.hpp>

#include "impl/type_traits/integral_constant.hpp"
#include "impl/type_traits/is_signed.hpp"
#include "impl/type_traits/is_unsigned.hpp"
#include "impl/type_traits/enable_if.hpp"
#include "impl/type_traits/make_signed.hpp"
#include "impl/type_traits/make_unsigned.hpp"
#include "impl/type_traits/remove_cv.hpp"

namespace lib {


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// void_t
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<class...>
using void_t = void;



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// add_const
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct add_const
{
	using type = const T;
};


template<class T>
using add_const_t = typename add_const<T>::type;



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// add_lvalue_reference
//
///////////////////////////////////////////////////////////////////////////////////////////////////
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



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// add_rvalue_reference
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
struct add_rvalue_reference
{
	using type = T&&;
};


template<class T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;





///////////////////////////////////////////////////////////////////////////////////////////////////
//
// remove_reference
//
///////////////////////////////////////////////////////////////////////////////////////////////////
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



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// declval
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
typename lib::add_rvalue_reference<T>::type declval() noexcept;



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// conditional
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<bool Cond, class T, class F>
struct conditional
{
	using type = T;
};


template<class T, class F>
struct conditional<false, T, F>
{
	using type = F;
};


template<bool Cond, class T, class F>
using conditional_t = typename conditional<Cond, T, F>::type;



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// is_pointer
//
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace _details
{

template<class T>
struct is_pointer_helper: false_type {};


template<class T>
struct is_pointer_helper<T *>: true_type {};

} // namespace _details


template<class T>
struct is_pointer: _details::is_pointer_helper<remove_cv_t<T>> {};


template<class T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// is_same
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template<class T1, class T2>
struct is_same: public false_type {};


template<class T>
struct is_same<T, T>: public true_type {};


template<class T1, class T2>
constexpr bool is_same_v = is_same<T1, T2>::value;


template<class T>
struct is_pod: integral_constant<bool, __is_pod(T)> {};

template<class T>
constexpr bool is_pod_v = is_pod<T>::value;

template<class T>
struct is_standard_layout: integral_constant<bool, __is_standard_layout(T)> {};

template<class T>
constexpr bool is_standard_layout_v = is_standard_layout<T>::value;

template<class T>
struct is_trivial: integral_constant<bool, __is_trivial(T)> {};

template<class T>
constexpr bool is_trivial_v = is_trivial<T>::value;

template<class T>
struct underlying_type {
	using type = __underlying_type(T);
};

template<class T>
using underlying_type_t = typename underlying_type<T>::type;


template<class T>
struct remove_extent { typedef T type; };

template<class T>
struct remove_extent<T[]> { typedef T type; };

template<class T, lib::size_t N>
struct remove_extent<T[N]> { typedef T type; };

template<class T>
using remove_extent_t = typename lib::remove_extent<T>::type;


template<class T>
struct is_enum: lib::integral_constant<bool, __is_enum(T)> {};

template<class T>
constexpr bool is_enum_v = lib::is_enum<T>::value;

} // namespace lib



