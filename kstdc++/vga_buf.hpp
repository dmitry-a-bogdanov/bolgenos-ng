#pragma once

#include <streambuf>


namespace lib {


namespace _impl {

class vga_buf: public std::streambuf {
public:
	vga_buf();
	virtual ~vga_buf();
protected:
	virtual int overflow(int c) override;
	virtual size_t xsputn(const char *s, size_t n) override;
};


} // namespace _impl


} // namespace lib
