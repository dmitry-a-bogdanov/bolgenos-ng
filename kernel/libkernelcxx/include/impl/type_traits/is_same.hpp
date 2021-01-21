#pragma once

namespace lib {

template<class T1, class T2>
struct is_same: public false_type {};


template<class T>
struct is_same<T, T>: public true_type {};


template<class T1, class T2>
constexpr bool is_same_v = is_same<T1, T2>::value;


}
