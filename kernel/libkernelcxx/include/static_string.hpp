#pragma once

#include "ostream.hpp"

namespace lib {

template<class Char, Char ...Chars>
struct basic_static_string{};

template<class Char, Char ...Chars>
lib::ostream& operator<<(lib::ostream& out, const basic_static_string<Char, Chars...>&) {
	return (out << ... << Chars);
}

template<class Char, Char ...Chars>
constexpr basic_static_string<Char, Chars...> operator""_ss() {
	return basic_static_string<Char, Chars...>{};
}

template<char ...Chars>
using static_string = lib::basic_static_string<char, Chars...>;

template<class Char, Char ...CharsL, Char ...CharsR>
constexpr auto operator+(
	const lib::basic_static_string<Char, CharsL...>&,
	const lib::basic_static_string<Char, CharsR...>&)
{
	return lib::basic_static_string<Char, CharsL..., CharsR...>{};
}

}

using lib::operator""_ss;
