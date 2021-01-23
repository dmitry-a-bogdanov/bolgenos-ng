#include "log/vga_log_buf.hpp"

#include <bolgenos-ng/vga_console.hpp>

using namespace vga_console;
using namespace lib;

log::VgaDelegatingLogBuf::VgaDelegatingLogBuf(LogLevel log_level, const char* prefix,
					      LogLevel& enabled_log_level, lib::streambuf* delegate,
					      vga_console::color_t color):
	DelegatingLogBuf(log_level, prefix, enabled_log_level, delegate),
	_expected_color{color}
{}

void log::VgaDelegatingLogBuf::handle_pre_prefix()
{
	_saved_color = vga_console::get_fg();
	set_fg(_expected_color);
}

void log::VgaDelegatingLogBuf::handle_end_line()
{
	set_fg(_saved_color);
}
