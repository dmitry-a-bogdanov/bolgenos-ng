#include "vga_log_buf.hpp"

#include <bolgenos-ng/io/vga/text_console.hpp>

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




lib::_impl::vga_log_buf::vga_log_buf(log_level_type log_level, const char* prefix)
	: log_level_(log_level), prefix_(prefix), show_header_(true) {
}


lib::_impl::vga_log_buf::~vga_log_buf() {
}


int lib::_impl::vga_log_buf::overflow(int c) {
	if (log_level_ > system_log_level)
		return c;
	if (show_header_) {
		bolgenos::io::vga::TextConsole::instance()->puts(prefix_);
		show_header_ = false;
	}
	bolgenos::io::vga::TextConsole::instance()->putc(c);
	if (c == '\n') {
		show_header_= true;
	}
	return c;
}
