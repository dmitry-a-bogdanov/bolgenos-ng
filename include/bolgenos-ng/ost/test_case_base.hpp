#pragma once

#include <bolgenos-ng/compiler.h>
#include "test_runner.hpp"

namespace ost
{

class BaseTest: public TestRunner::test_queue_node
{
public:
	BaseTest() = delete;
	BaseTest(const BaseTest&) = delete;
	BaseTest& operator=(const BaseTest&) = delete;

	BaseTest(const char* suite_name, const char* case_name, TestRunner* runner = nullptr);

	virtual ~BaseTest();

	const char* getSuiteName() const;
	const char* getCaseName() const;

	void execute();
protected:
	virtual void run() const = 0;
private:
	const char* _suite_name;
	const char* _case_name;
	TestRunner* _runner;
};

}

#define OST_TEST_CLSNAME(ts, tc) TestCaseDeclaration_ ## ts ## __ ## tc ## __

#define TEST(suite_name, case_name) \
class OST_TEST_CLSNAME(suite_name, case_name): public ::ost::BaseTest \
{ \
public: \
	OST_TEST_CLSNAME(suite_name, case_name) () = delete; \
	OST_TEST_CLSNAME(suite_name, case_name) (const char *ts, const char *tc) \
		: BaseTest(ts, tc, nullptr) \
	{} \
	virtual ~OST_TEST_CLSNAME(suite_name, case_name) () = default; \
protected: \
	void run() const override; \
} OST_TEST_CLSNAME(suite_name, case_name) { \
		stringify(suite_name), \
		stringify(case_name) \
}; \
void OST_TEST_CLSNAME(suite_name, case_name)::run() const

