#include "include/log.hpp"

#include "vga_buf.hpp"
#include "vga_log_buf.hpp"

using namespace lib;
using namespace vga_console;

namespace {

LogLevel global_log_level;

VgaBuf plain_vga_buf;
VgaLogBuf crit_buf(LogLevel::CRITICAL, "<none>: ", global_log_level, color_t::red);
VgaLogBuf err_buf(LogLevel::ERROR, "<none>: ", global_log_level, color_t::bright_red);
VgaLogBuf warn_buf(LogLevel::WARNING, "<none>: ", global_log_level, color_t::yellow);
VgaLogBuf notice_buf(LogLevel::NOTICE, "<none>: ", global_log_level, color_t::green);
VgaLogBuf info_buf(LogLevel::INFO, "<none>: ", global_log_level, color_t::bright_green);


} // namespace

void lib::set_log_level(LogLevel log_level) {
	global_log_level = log_level;
}


lib::LogLevel lib::get_log_level() {
	return global_log_level;
}

lib::ostream lib::cout(&plain_vga_buf);
lib::ostream lib::ccrit(&crit_buf);
lib::ostream lib::cerr(&err_buf);
lib::ostream lib::cwarn(&warn_buf);
lib::ostream lib::cnotice(&notice_buf);
lib::ostream lib::cinfo(&info_buf);
