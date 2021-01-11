#pragma once

#include "integral_constant.hpp"

namespace lib {


template<typename Type>
struct is_unsigned: false_type {};


template<>
struct is_unsigned<unsigned long long>: true_type {};


template<>
struct is_unsigned<unsigned long>: true_type {};


template<>
struct is_unsigned<unsigned int>: true_type {};


template<>
struct is_unsigned<unsigned short>: true_type {};


template<>
struct is_unsigned<unsigned char>: true_type {};


} // namespace lib
