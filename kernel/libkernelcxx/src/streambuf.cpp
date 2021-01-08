#include "streambuf.hpp"


lib::streambuf::~streambuf() {

}

int lib::streambuf::sputc(char c) {
	return overflow(c);
}


size_t lib::streambuf::sputn(const char* s, size_t n) {
	return xsputn(s, n);
}

