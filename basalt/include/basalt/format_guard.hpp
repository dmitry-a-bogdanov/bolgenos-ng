#pragma once

#include <ostream>

namespace basalt {

class scoped_format_guard {
public:
	scoped_format_guard() = delete;
	scoped_format_guard(const scoped_format_guard&) = delete;

	scoped_format_guard(std::ostream& guarded_stream);

	virtual ~scoped_format_guard();
private:
	std::ostream& _guarded_stream;
	std::ostream _backup;
};

}
