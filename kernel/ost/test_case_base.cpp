#include <bolgenos-ng/ost/test_case_base.hpp>

#include <bolgenos-ng/log.hpp>
#include <bolgenos-ng/ost/conf.hpp>

namespace ost
{

BaseTest::BaseTest(const char* suite_name, const char* case_name, TestRunner* runner)
	: TestRunner::test_queue_node(this),
	_suite_name(suite_name), _case_name(case_name), _runner(runner)
{
	if (!_runner)
	{
		_runner = TestRunner::default_runner();
	}
	_runner->register_test_case(this);
}

BaseTest::~BaseTest() = default;

const char* BaseTest::getSuiteName() const { return _suite_name; }

const char* BaseTest::getCaseName() const { return _case_name; }

void BaseTest::execute()
{
	_LOG_STREAM(Conf::debug_stream(), "[" << getSuiteName() << "::"
		<< getCaseName() << "]: started");
	this->run();
	_LOG_STREAM(Conf::debug_stream(), "[" << getSuiteName() << "::"
	<< getCaseName() << "]: finished");
}

}
