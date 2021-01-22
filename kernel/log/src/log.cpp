#include <log.hpp>

#include "vga_buf.hpp"
#include "vga_log_buf.hpp"
#include "composite_buf.hpp"
#include "serial_buf.hpp"

using namespace lib;
using namespace vga_console;

namespace {

struct {
	LogLevel vga{LogLevel::NOTICE};
	LogLevel serial{LogLevel::INFO};
} conf{};

constexpr color_t color(LogLevel level) {
	switch (level)
	{
	case LogLevel::CRITICAL: return red;
	case LogLevel::ERROR: return bright_red;
	case LogLevel::WARNING: return yellow;
	case LogLevel::NOTICE: return green;
	case LogLevel::INFO: return bright_green;
	default: panic("Unknown log level");
	}
}

VgaLogBuf build_global_buf(LogLevel level) {
	return {level, "<none>: ", conf.vga, color(level)};
}

VgaBuf plain_vga_buf;
VgaLogBuf crit_buf = build_global_buf(LogLevel::CRITICAL);
VgaLogBuf err_buf = build_global_buf(LogLevel::ERROR);
VgaLogBuf warn_buf = build_global_buf(LogLevel::WARNING);

CompositeBuf<VgaLogBuf, SerialLogBuf> notice_buf{
	build_global_buf(LogLevel::NOTICE)
};

VgaLogBuf info_buf = build_global_buf(LogLevel::INFO);


} // namespace

void lib::set_log_level(LogLevel log_level) {
	conf.vga = log_level;
}


lib::LogLevel lib::get_log_level() {
	return conf.vga;
}

lib::ostream lib::cout(&plain_vga_buf);
lib::ostream lib::ccrit(&crit_buf);
lib::ostream lib::cerr(&err_buf);
lib::ostream lib::cwarn(&warn_buf);
lib::ostream lib::cnotice(&notice_buf);
lib::ostream lib::cinfo(&info_buf);

void lib::set_serial_port_for_logging(serial::SerialPort serial_port)
{
	notice_buf.set(SerialLogBuf{LogLevel::NOTICE, "<none>: ", conf.serial, lib::move(serial_port)});
}
