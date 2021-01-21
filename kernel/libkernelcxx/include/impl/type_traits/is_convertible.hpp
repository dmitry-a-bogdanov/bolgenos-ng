#pragma once

#include "integral_constant.hpp"
#include "void.hpp"

namespace lib {

namespace impl {

template<class T>
auto test_returnable(int) -> decltype(
void(static_cast<T(*)()>(nullptr)), lib::true_type{}
);
template<class>
auto test_returnable(...) -> lib::false_type;

template<class From, class To>
auto test_implicitly_convertible(int) -> decltype(
void(lib::declval<void(&)(To)>()(lib::declval<From>())), lib::true_type{}
);
template<class, class>
auto test_implicitly_convertible(...) -> lib::false_type;

} // namespace detail

template<class From, class To>
struct is_convertible : lib::integral_constant<bool,
	(decltype(impl::test_returnable<To>(0))::value &&
	 decltype(impl::test_implicitly_convertible<From, To>(0))::value) ||
	(lib::is_void_v<From> && lib::is_void_v<To>)
> {};

template< class From, class To >
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

} // namespace lib
