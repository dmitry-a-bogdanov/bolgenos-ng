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


} // namespace

void lib::set_log_level(LogLevel log_level) {
	conf.enabled_log_level = log_level;
}


lib::LogLevel lib::get_log_level() {
	return conf.enabled_log_level;
}
