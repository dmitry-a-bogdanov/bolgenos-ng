#pragma once

#include <ostream.hpp>

namespace lib {

class ScopedFormatGuard
{
public:
	ScopedFormatGuard() = delete;
	ScopedFormatGuard(const ScopedFormatGuard&) = delete;
	ScopedFormatGuard(ScopedFormatGuard&&) = delete;
	ScopedFormatGuard& operator=(const ScopedFormatGuard) = delete;
	ScopedFormatGuard& operator=(ScopedFormatGuard&&) = delete;

	ScopedFormatGuard(ostream& guarded_stream);

	~ScopedFormatGuard();
public:
	ostream& guarded_stream_;
	ostream backup_;
};

}
