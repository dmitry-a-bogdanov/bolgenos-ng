#pragma once

#include <lib/ostream.hpp>

namespace basalt {

class scoped_format_guard {
public:
	scoped_format_guard() = delete;
	scoped_format_guard(const scoped_format_guard&) = delete;

	scoped_format_guard(lib::ostream& guarded_stream);

	virtual ~scoped_format_guard();
private:
	lib::ostream& _guarded_stream;
	lib::ostream _backup;
};

}
