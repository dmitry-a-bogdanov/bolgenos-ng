#include <bolgenos-ng/ost/test_runner.hpp>
#include <bolgenos-ng/ost.hpp>
namespace ost
{

TestRunner* TestRunner::default_runner()
{
	static TestRunner default_runner_instance;
	return &default_runner_instance;
}

void TestRunner::register_test_case(BaseTest* test_case)
{
	_tests.push_front(*test_case);
	++_registered_tests;
}

void TestRunner::execute() const
{
	for (auto& test: _tests)
	{
		test->execute();
	}
}

}
