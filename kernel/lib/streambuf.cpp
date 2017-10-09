#include "streambuf.hpp"


std::streambuf::~streambuf() {

}

int std::streambuf::sputc(char c) {
	return overflow(c);
}


size_t std::streambuf::sputn(const char* s, size_t n) {
	return xsputn(s, n);
}

