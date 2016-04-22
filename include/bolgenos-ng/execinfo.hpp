#pragma once

#include <lib/ostream.hpp>


namespace execinfo {


void show_backtrace(lib::ostream& out, void *ebp = nullptr,
		void *eip = nullptr);


} // namespace execinfo
