#pragma once


namespace lib {


/// \brief Swap objects.
///
/// Exchange values of two objects.
///
/// \param a The first object, which content is swapped.
/// \param b The second object, which content is swapped.
template <class ValueType> void swap (ValueType& a, ValueType& b) {
	ValueType tmp(a);
	a = b;
	b = tmp;
}


} // namespace lib
