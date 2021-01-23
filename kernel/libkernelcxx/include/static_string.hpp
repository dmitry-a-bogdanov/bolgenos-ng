#pragma once

#include "ostream.hpp"

namespace lib {

template<char ...Chars>
struct static_string{};

template<char ...Chars>
lib::ostream& operator<<(lib::ostream& out, const static_string<Chars...>&) {
	return out << ... << Chars;
}

}
