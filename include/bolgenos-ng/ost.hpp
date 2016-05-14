#pragma once


#include <bolgenos-ng/error.h>

#include <lib/ostream.hpp>


namespace ost {


void run();


inline void __show() {
	lib::ccrit << lib::endl;
}


template<typename HeadType, typename ...TailTypes>
void __show(const HeadType &head, TailTypes ...tail) {
	lib::ccrit << head;
	__show(tail...);
}


template<typename ...Messages>
void assert(bool expression, Messages... messages) {
	if (not expression) {
		__show(messages...);
		panic("OST assertion failed.");
	}
}

#define OST_ASSERT(expression, ...) \
	ost::assert((expression), __FILE__, ":", __LINE__, ": ", ##__VA_ARGS__)

} // namespace ost
