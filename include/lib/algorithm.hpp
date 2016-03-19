#pragma once


namespace lib {


template<typename ForwardIteratorType, typename EndIteratorType,
		typename FunctionType>
void for_each(ForwardIteratorType first, EndIteratorType last,
		FunctionType func) {
	for(; first != last; ++first)
		func(*first);
}


} // namespace lib
