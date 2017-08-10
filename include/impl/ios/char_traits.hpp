#pragma once

#include "types.hpp"

namespace std
{

template<class T>
struct char_traits;

template<>
struct char_traits<char>
{
	using char_type = char;
	using int_type = int;
	using off_type = std::streamoff;
	using pos_type = std::streampos;


	static constexpr int_type to_int_type(char_type ch) noexcept
	{
		return int_type{ch};
	}


	static constexpr int_type eof() noexcept
	{
		return -1;
	}
};


}
// namespace std
