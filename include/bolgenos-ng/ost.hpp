#pragma once


#include <bolgenos-ng/error.h>
#include <bolgenos-ng/log.hpp>

namespace ost {


void run();


inline void show__() {
	lib::ccrit << lib::endl;
}


template<typename HeadType, typename ...TailTypes>
void show__(const HeadType &head, TailTypes ...tail) {
	lib::ccrit << head;
	show__(tail...);
}


template<typename ...Messages>
void assert(bool expression, Messages... messages) {
	if (not expression) {
		show__(messages...);
		panic("OST assertion failed.");
	}
}

#define OST_ASSERT(expression, ...) \
	ost::assert((expression), __FILE__, ":", __LINE__, ": ", ##__VA_ARGS__)

} // namespace ost
