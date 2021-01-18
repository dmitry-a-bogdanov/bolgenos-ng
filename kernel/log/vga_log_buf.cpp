#include "vga_log_buf.hpp"

#include <bolgenos-ng/vga_console.hpp>

using namespace vga_console;
using namespace lib;

VgaLogBuf::VgaLogBuf(LogLevel log_level, const char* prefix,
		     LogLevel& enabled_log_level, color_t color) :
	log_level_(log_level),
	prefix_(prefix),
	show_header_(true),
	expected_color_{color},
	saved_color_{color},
	enabled_log_level_{enabled_log_level}
{}

VgaLogBuf::~VgaLogBuf() {
}

int VgaLogBuf::overflow(int c) {
	if (log_level_ > enabled_log_level_)
		return c;
	if (show_header_) {
		saved_color_ = vga_console::get_fg();
		set_fg(expected_color_);
		vga_console::puts(prefix_);
		show_header_ = false;
	}
	vga_console::putc(c);
	if (c == '\n') {
		set_fg(saved_color_);
		show_header_= true;
	}
	return c;
}
