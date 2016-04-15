#pragma once


namespace lib {


template <class ValueType> void swap (ValueType& a, ValueType& b) {
	ValueType tmp(a);
	a = b;
	b = tmp;
}

} // namespace lib
