#pragma once

#include <log.hpp>

#include "vga_buf.hpp"
#include "base_log_buf.hpp"
#include <bolgenos-ng/vga_console.hpp>

class VgaLogBuf: public BaseLogBuf<VgaBuf> {
public:

	VgaLogBuf() = delete;
	VgaLogBuf(const VgaLogBuf&) = delete;
	VgaLogBuf(VgaLogBuf&&);

	VgaLogBuf& operator=(const VgaLogBuf&) = delete;
	VgaLogBuf(lib::LogLevel log_level, const char* prefix,
		  lib::LogLevel& enabled_log_level,
		  vga_console::color_t color = vga_console::color_t::white);
	~VgaLogBuf() override;
protected:
	using base_type = BaseLogBuf<VgaBuf>;

	void handle_pre_prefix() override;

	void handle_end_line() override;

private:
	const vga_console::color_t _expected_color;
	vga_console::color_t _saved_color{_expected_color};
};
