#include "vga_log_buf.hpp"

#include <bolgenos-ng/vga_console.hpp>

using namespace vga_console;

lib::log_level_type
lib::_impl::vga_log_buf::system_log_level = lib::log_level_type::info;


lib::log_level_type
lib::_impl::vga_log_buf::get_system_log_level() {
	return system_log_level;
}


void
lib::_impl::vga_log_buf::set_system_log_level(log_level_type log_level) {
	system_log_level = log_level;
}




lib::_impl::vga_log_buf::vga_log_buf(log_level_type log_level, const char* prefix,
				     color_t color)
	: log_level_(log_level), prefix_(prefix), show_header_(true),
	expected_color_{color}, saved_color_{color} {
}


lib::_impl::vga_log_buf::~vga_log_buf() {
}


int lib::_impl::vga_log_buf::overflow(int c) {
	if (log_level_ > system_log_level)
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
