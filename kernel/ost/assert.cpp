#include <bolgenos-ng/ost/assert.hpp>

#include <bolgenos-ng/error.h>

namespace ost
{

Assert::Assert(std::ostream& good_ostream, std::ostream& bad_ostream, bool cond,
	const char* expr, const char* testsuite, const char *testcase, int line)
	: _good_ostream { good_ostream },
	_bad_ostream { bad_ostream },
	_cond { cond },
	_expr { expr },
	_suite { testsuite },
	_case { testcase },
	_line { line }
{
}

Assert::~Assert()
{
	if (!_already_printed)
		print_preamble();
	cond_stream() << std::endl;

	if (!_cond)
		panic("Test failed");
}

std::ostream& Assert::cond_stream()
{
	if (_cond)
		return _good_ostream;
	else
		return _bad_ostream;
}

void Assert::print_preamble()
{
	cond_stream() << _suite << "::" << _case << ": " << _line << ": "
		<< "assertion(" << _expr << ") " << (_cond ? "passed" : "failed");
}

}

