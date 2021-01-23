#include <log.hpp>

#include <log/vga_log_buf.hpp>
#include <log/composite_buf.hpp>
#include <log/serial_buf.hpp>
#include "buf_builder.hpp"

using namespace lib;
using namespace vga_console;

namespace {

struct {
	LogLevel enabled_log_level{LogLevel::INFO};
} conf{};


CompositeBuf<log::VgaDelegatingLogBuf, log::SerialDelegatingLogBuf> build_global_buf(LogLevel level) {
	return log::build_global_buf(level, "<none> :", conf.enabled_log_level);
}

auto crit_buf = build_global_buf(LogLevel::CRITICAL);
auto err_buf = build_global_buf(LogLevel::ERROR);
auto warn_buf = build_global_buf(LogLevel::WARNING);
auto notice_buf = build_global_buf(LogLevel::NOTICE);
auto info_buf = build_global_buf(LogLevel::INFO);


} // namespace

void lib::set_log_level(LogLevel log_level) {
	conf.enabled_log_level = log_level;
}


lib::LogLevel lib::get_log_level() {
	return conf.enabled_log_level;
}

lib::ostream lib::ccrit(&crit_buf);
lib::ostream lib::cerr(&err_buf);
lib::ostream lib::cwarn(&warn_buf);
lib::ostream lib::cnotice(&notice_buf);
lib::ostream lib::cinfo(&info_buf);

