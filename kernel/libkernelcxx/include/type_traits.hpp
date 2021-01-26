#pragma once

#include <cstddef.hpp>

#include "impl/type_traits/add_reference.hpp"
#include "impl/type_traits/declval.hpp"
#include "impl/type_traits/integral_constant.hpp"
#include "impl/type_traits/is_signed.hpp"
#include "impl/type_traits/is_unsigned.hpp"
#include "impl/type_traits/enable_if.hpp"
#include "impl/type_traits/make_signed.hpp"
#include "impl/type_traits/make_unsigned.hpp"
#include "impl/type_traits/remove_cv.hpp"
#include "impl/type_traits/is_convertible.hpp"
#include "impl/type_traits/void.hpp"
#include "impl/type_traits/is_same.hpp"

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

template<typename Base, typename Derived>
struct is_base_of: lib::integral_constant<bool, __is_base_of(Base, Derived)> {};

template< class Base, class Derived >
inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;


template<lib::size_t Len, lib::size_t Align = alignof(int)>
struct aligned_storage {
	struct type {
		alignas(Align) unsigned char data[Len];
	};
};

template<lib::size_t Len, lib::size_t Align = alignof(int)>
using aligned_storage_t = typename lib::aligned_storage<Len, Align>::type;


template<class T>
struct is_empty: lib::integral_constant<bool, __is_empty(T)> {};

template<class T>
constexpr inline bool is_empty_v = lib::is_empty<T>::value;

} // namespace lib



