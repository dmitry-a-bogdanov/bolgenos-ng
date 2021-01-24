#pragma once


#include <bolgenos-ng/error.h>
#include <bolgenos-ng/compiler.h>
#include <logger.hpp>

namespace ost {


using test_caller = void ();

void run();

template<typename ...Messages>
void assert(bool expression, Messages... messages) {
	LOCAL_LOGGER("ost_assert", lib::LogLevel::INFO);
	if (not expression) {
		(lib::ccrit << ... << messages);
		panic("On start test failed");
	}
}

} // namespace ost


#define OST_ASSERT(expression, ...) \
	::ost::assert((expression), __FILE__, ":", __LINE__, ": ", ##__VA_ARGS__)

#define TEST(ts, tc) \
namespace tests::ts { \
void tc(); \
} \
namespace test_callers::ts { \
void tc() { \
        LOCAL_LOGGER("ost", lib::LogLevel::INFO); \
	LOG_INFO << stringify(ts) << "::" << stringify(tc) << ": starting" << lib::endl; \
	tests::ts::tc(); \
	LOG_INFO << stringify(ts) << "::" << stringify(tc) << ": ok" << lib::endl; \
} \
ost::test_caller* caller_ ## tc [[gnu::section(".ost_callers"), gnu::used]] = test_callers::ts::tc; \
} \
void tests::ts::tc()

