#pragma once
#include <ostream>
#include <basalt/intrusive_list.hpp>


namespace ost
{

class BaseTest;

class TestRunner
{
public:
	using test_queue_type = basalt::intrusive_list<BaseTest *>;
	using test_queue_node = basalt::intrusive_list_node<BaseTest *>;

	static TestRunner* default_runner();

	void register_test_case(BaseTest* test_case);
	void execute() const;

private:
	test_queue_type _tests{};
	size_t _registered_tests{ 0 };
};


}
