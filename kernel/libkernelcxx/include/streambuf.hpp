#pragma once

#include <cstddef.hpp>


namespace lib {

class streambuf {
public:
	static constexpr int end_of_file = -1;

	streambuf() = default;
	streambuf(const streambuf&) = delete;
	streambuf(streambuf&&) = default;
	streambuf& operator=(const streambuf&) = delete;
	virtual ~streambuf();

	int sputc(char c);
	size_t sputn(const char *s, size_t n);
protected:
	virtual int overflow(int c) = 0;
	virtual size_t xsputn(const char *s, size_t n) = 0;
private:
}; // class streambuf


} // namespace lib
