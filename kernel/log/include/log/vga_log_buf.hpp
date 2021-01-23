#pragma once

#include <log.hpp>

#include "delegating_log_buf.hpp"
#include <bolgenos-ng/vga_console.hpp>

namespace log {

class VgaDelegatingLogBuf: public log::DelegatingLogBuf {
public:
	VgaDelegatingLogBuf(lib::LogLevel log_level, const char* prefix,
			    lib::LogLevel& enabled_log_level, lib::streambuf* delegate,
			    vga_console::color_t color = vga_console::color_t::white);
	VgaDelegatingLogBuf(VgaDelegatingLogBuf&&) = default;

protected:
	void handle_pre_prefix() override;

	void handle_end_line() override;

private:
	const vga_console::color_t _expected_color;
	vga_console::color_t _saved_color{_expected_color};
};

}