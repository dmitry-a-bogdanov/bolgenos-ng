#pragma once

#include "integral_constant.hpp"

namespace std {


template<typename Type>
struct is_signed: false_type {};


template<>
struct is_signed<long long>: true_type {};


template<>
struct is_signed<long>: true_type {};


template<>
struct is_signed<int>: true_type {};


template<>
struct is_signed<short>: true_type {};


template<>
struct is_signed<char>: true_type {};


} // namespace std
