#include "vga_log_buf.hpp"

#include <bolgenos-ng/vga_console.hpp>

#include <utility.hpp>

using namespace vga_console;
using namespace lib;

VgaLogBuf::VgaLogBuf(VgaLogBuf&& other)
	: BaseLogBuf<VgaBuf>(lib::move(other))
	, _expected_color(other._expected_color)
	, _saved_color(other._saved_color)
{}

VgaLogBuf::VgaLogBuf(LogLevel log_level, const char* prefix,
		     LogLevel& enabled_log_level, color_t color)
	: BaseLogBuf<VgaBuf>{log_level, prefix, enabled_log_level}
	, _expected_color{color}
{}

void VgaLogBuf::handle_pre_prefix()
{
	_saved_color = vga_console::get_fg();
	set_fg(_expected_color);
}

void VgaLogBuf::handle_end_line()
{
	set_fg(_saved_color);
}

VgaLogBuf::~VgaLogBuf() = default;

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
