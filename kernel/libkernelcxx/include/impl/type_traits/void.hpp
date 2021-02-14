#pragma once

#include "is_same.hpp"

namespace lib {

template<class T>
struct is_void: lib::is_same<void, typename lib::remove_cv<T>::type> {};

template<class T>
inline constexpr bool is_void_v = lib::is_void<T>::value;

}
