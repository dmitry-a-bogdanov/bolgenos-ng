#pragma once

#include "streambuf.hpp"


namespace lib {


namespace _impl {

class vga_buf: public streambuf {
public:
	vga_buf();
	virtual ~vga_buf();
protected:
	virtual int overflow(int c);
	virtual size_t xsputn(const char *s, size_t n);
};


} // namespace _impl


} // namespace lib
