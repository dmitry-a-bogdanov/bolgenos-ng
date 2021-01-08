#include <bolgenos-ng/log.hpp>

#include "vga_buf.hpp"
#include "vga_log_buf.hpp"


void lib::set_log_level(log_level_type log_level) {
	lib::_impl::vga_log_buf::set_system_log_level(log_level);
}


lib::log_level_type lib::get_log_level() {
	return lib::_impl::vga_log_buf::get_system_log_level();
}

namespace {


lib::_impl::vga_buf plain_vga_buf;
lib::_impl::vga_log_buf crit_buf(lib::log_level_type::critical, "[CRIT] ");
lib::_impl::vga_log_buf err_buf(lib::log_level_type::error, "[ERR] ");
lib::_impl::vga_log_buf warn_buf(lib::log_level_type::warning, "[WARN] ");
lib::_impl::vga_log_buf notice_buf(lib::log_level_type::notice, "[NOTE] ");
lib::_impl::vga_log_buf info_buf(lib::log_level_type::info, "[info] ");


} // namespace

lib::ostream lib::cout(&plain_vga_buf);
lib::ostream lib::ccrit(&crit_buf);
lib::ostream lib::cerr(&err_buf);
lib::ostream lib::cwarn(&warn_buf);
lib::ostream lib::cnotice(&notice_buf);
lib::ostream lib::cinfo(&info_buf);
