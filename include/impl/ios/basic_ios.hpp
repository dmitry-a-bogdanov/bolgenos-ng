#pragma once

#include <streambuf>
#include <string>
#include <utility>

#include "ios_base.hpp"

namespace std
{


template<class CharT, class Traits = std::char_traits<CharT>>
class basic_ios: public ios_base
{
public:
	using char_type = CharT;
	using traits_type = Traits;
	using int_type = typename traits_type::int_type;
	using pos_type = typename traits_type::pos_type;
	using off_type = typename traits_type::off_type;

	virtual ~basic_ios()
	{
		_streambuf = nullptr;
	}

	explicit basic_ios(std::basic_streambuf<CharT, Traits> *sb)
		: ios_base(),
		_streambuf(sb) {}

	basic_ios(const basic_ios&) = delete;
	basic_ios& operator=(const basic_ios&) = delete;

	basic_ios& copyfmt(const basic_ios&);
	/*{
		return *this;
	}*/

	CharT fill() const;
	CharT fill(CharT ch);

	std::basic_streambuf<CharT, Traits>* rdbuf() const
	{
		return _streambuf;
	}

	std::basic_streambuf<CharT, Traits>* rdbuf(std::basic_streambuf<CharT, Traits>* sb)
	{
		std::swap(sb, _streambuf);
		return sb;
	}

protected:
	basic_ios();

private:
	std::basic_streambuf<CharT, Traits>* _streambuf{nullptr};

};


} // namespace std
