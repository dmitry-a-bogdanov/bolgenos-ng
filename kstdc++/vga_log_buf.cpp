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

lib::_impl::vga_log_buf crit_buf(lib::log_level_type::critical, "[C] ");
lib::_impl::vga_log_buf err_buf(lib::log_level_type::error, "[E] ");
lib::_impl::vga_log_buf warn_buf(lib::log_level_type::warning, "[W] ");
lib::_impl::vga_log_buf notice_buf(lib::log_level_type::notice, "[N] ");
lib::_impl::vga_log_buf info_buf(lib::log_level_type::info, "[I] ");
lib::_impl::vga_log_buf debug_buf(lib::log_level_type::debug, "[D] ");


void lib::set_log_level(log_level_type log_level) {
	lib::_impl::vga_log_buf::set_system_log_level(log_level);
}


lib::log_level_type lib::get_log_level() {
	return lib::_impl::vga_log_buf::get_system_log_level();
}

std::ostream lib::ccrit(&crit_buf);
std::ostream lib::cerr(&err_buf);
std::ostream lib::cwarn(&warn_buf);
std::ostream lib::cnotice(&notice_buf);
std::ostream lib::cinfo(&info_buf);
std::ostream lib::cdebug(&debug_buf);
