#pragma once

#include <ostream>
#include "conf.hpp"

namespace ost
{

class Assert final
{
public:
	Assert() = delete;
	Assert(const Assert&) = delete;
	Assert& operator=(Assert&) = delete;

	explicit Assert(std::ostream& good_ostream, std::ostream& bad_ostream, bool cond,
		const char* expr, const char* testsuite, const char *testcase, int line);

	~Assert();

	template<class T>
	std::ostream& operator<<(const T& arg)
	{
		_already_printed = true;
		print_preamble();
		return cond_stream() << ": " << arg;
	}

protected:
	std::ostream& cond_stream();
	void print_preamble();

private:
	std::ostream& _good_ostream;
	std::ostream& _bad_ostream;
	bool _cond;
	const char* _expr;
	const char* _suite;
	const char* _case;
	int _line;

	bool _already_printed { false };
};


#define OST_ASSERT(expr) \
	::ost::Assert(::ost::Conf::good_stream(), \
			::ost::Conf::bad_stream(), \
			(expr), \
			stringify(expr), \
			getSuiteName(), \
			getCaseName(), \
			__LINE__)

#define OST_ASSERT_EQ(x, y) OST_ASSERT((x) == (y))

}
