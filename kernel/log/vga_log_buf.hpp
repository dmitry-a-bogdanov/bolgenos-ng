#pragma once

#include <log.hpp>

#include "vga_buf.hpp"
#include <bolgenos-ng/vga_console.hpp>

class VgaLogBuf: public VgaBuf {
public:

	VgaLogBuf() = delete;
	VgaLogBuf(const VgaLogBuf&) = delete;
	VgaLogBuf& operator=(const VgaLogBuf&) = delete;
	VgaLogBuf(lib::LogLevel log_level, const char* prefix,
		  lib::LogLevel& enabled_log_level,
		  vga_console::color_t color = vga_console::color_t::white);
	~VgaLogBuf() override;
protected:
	int overflow(int c) override;
private:
	lib::LogLevel log_level_;
	const char *prefix_;
	bool show_header_;

	const vga_console::color_t expected_color_;
	vga_console::color_t saved_color_;
	lib::LogLevel& enabled_log_level_;
};
