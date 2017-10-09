#pragma once

#include "basic_ios.hpp"

namespace std {

template<class CharT, class Traits = std::char_traits<CharT>>
class basic_ostream: virtual public basic_ios<CharT, Traits>
{
	using _basic_ios = basic_ios<CharT, Traits>;
public:
	class sentry;

	explicit basic_ostream(std::basic_streambuf<CharT, Traits> *sb)
		: basic_ios<CharT, Traits>(sb)
	{};

	using char_type = typename _basic_ios::char_type;
	using traits_type = typename _basic_ios::traits_type;

	basic_ostream& put(char_type ch)
	{
		_basic_ios::rdbuf()->sputc(ch);
		return *this;
	}


	basic_ostream& write(const char_type* s, std::streamsize count)
	{
		for(std::streamsize i = 0; i != count; ++i) put(s[i]);
		return *this;
	}

	basic_ostream& operator <<(short value)
	{
		return *this;
	}

	basic_ostream& operator <<(unsigned short value)
	{
		return *this;
	}


	basic_ostream& operator <<(int value)
	{
		if (value < 0)
		{
			put('-');
			value = -value;
		}

		constexpr std::size_t digits10 = 15;
		char repr[digits10];
		std::size_t i = 0;
		do
		{
			repr[i++] = '0' + (value % 10);
			value = value / 10;
		} while(value);

		write(repr, i);

		return *this;
	}

	basic_ostream& operator <<(unsigned int value)
	{
		return *this;
	}

	basic_ostream& operator <<(long value)
	{
		return *this;
	}

	basic_ostream& operator <<(unsigned long value)
	{
		return *this;
	}

	basic_ostream& operator <<(long long value)
	{
		return *this;
	}

	basic_ostream& operator <<(unsigned long long value)
	{
		return *this;
	}

	basic_ostream& operator <<(bool value)
	{
		if (ios_base::flags() & ios_base::boolalpha)
		{
			if (value)
				*this << "true";
			else
				*this << "false";
		}
		else
		{
			*this << static_cast<int>(value);
		}
		return *this;
	}

	basic_ostream& operator <<(std::nullptr_t)
	{
		return *this;
	}

	basic_ostream& operator <<(std::basic_streambuf<CharT, Traits>* sb)
	{
		return *this;
	}

	basic_ostream& operator <<(std::ios_base& (*f)(std::ios_base&))
	{
		f(*this);
		return *this;
	}

	basic_ostream& operator <<(std::basic_ios<CharT, Traits>& (*f)(std::basic_ios<CharT, Traits>&))
	{
		f(*this);
		return *this;
	}

	basic_ostream& operator <<(std::basic_ostream<CharT, Traits>& (*f)(std::basic_ostream<CharT, Traits>&))
	{
		return f(*this);
	}

	basic_ostream& flush();

protected:
	basic_ostream(const basic_ostream&) = delete;
	basic_ostream(basic_ostream&& rhs);

	basic_ostream& operator =(const basic_ostream& rhs) = delete;
	basic_ostream& operator =(basic_ostream&& rhs);

	void swap(basic_ostream& rhs);
};


}
