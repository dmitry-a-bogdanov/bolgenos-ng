#pragma once

#include <bolgenos-ng/vga_console.hpp>

namespace cio {


class OutStream {
public:
	using format_func_type = OutStream& (OutStream&);

	OutStream() {
	}

	virtual ~OutStream() {
	}

	OutStream& operator<<(const char *string);
	OutStream& operator<<(void *ptr);

	OutStream& operator<<(signed long val);
	OutStream& operator<<(unsigned long val);
	OutStream& operator<<(signed int val);
	OutStream& operator<<(unsigned int val);

	OutStream& operator<<(format_func_type formatter);
};


OutStream& endl(OutStream &stream);

extern OutStream cout;

} // namespace cio
