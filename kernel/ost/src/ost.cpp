#include <bolgenos-ng/ost.hpp>

#include <logger.hpp>

_asm_linked_ ost::test_caller* ost_callers_begin_label[0];
_asm_linked_ ost::test_caller* ost_callers_end_label[0];

using namespace lib;

LOCAL_LOGGER("ost", LogLevel::NOTICE);

void ost::run() {
	LOG_NOTICE << "Running " << ost_callers_end_label - ost_callers_begin_label << " tests" << endl;
	LOG_INFO << "first: " << ost_callers_begin_label << ", last :" << ost_callers_end_label << endl;
	for (auto it = ost_callers_begin_label; it != ost_callers_end_label; ++it) {
		(*it)();
	}
}
