#pragma once

#include <concepts.hpp>
#include <streambuf.hpp>
#include <utility.hpp>

template<lib::derived_from<lib::streambuf> BaseBuf>
class SimpleStreamBuf: public BaseBuf
{
public:
	SimpleStreamBuf() = default;

	template<class ...Args>
	SimpleStreamBuf(Args ...args)
		: BaseBuf(lib::forward<Args>(args)...)
	{}

	SimpleStreamBuf(SimpleStreamBuf&&) = default;
protected:
	size_t xsputn(const char* s, size_t n) override {
		for (size_t cnt = 0; cnt != n; ++cnt) {
			this->overflow(s[cnt]);
		}
		return n;
	}
};

