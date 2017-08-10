#pragma once

namespace std
{


template<class T>
struct underlying_type
{
#ifdef __GNUG__
	using type = __underlying_type(T);
#else
#error "Only GCC is supported"
#endif
};

template<class T>
using undelying_type_t = typename underlying_type<T>::type;


}
