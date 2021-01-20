#pragma once

#include "streambuf.hpp"

class VgaBuf: public lib::streambuf {
public:
	VgaBuf();
	~VgaBuf() override;
protected:
	int overflow(int c) override;
	size_t xsputn(const char *s, size_t n) override;
};
