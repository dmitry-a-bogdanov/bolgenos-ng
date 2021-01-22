#pragma once

#include "vga_log_buf.hpp"
#include "serial_buf.hpp"
#include "composite_buf.hpp"
#include <log.hpp>

namespace log {

constexpr vga_console::color_t color(lib::LogLevel level) {
	switch (level)
	{
	case lib::LogLevel::CRITICAL: return vga_console::red;
	case lib::LogLevel::ERROR: return vga_console::bright_red;
	case lib::LogLevel::WARNING: return vga_console::yellow;
	case lib::LogLevel::NOTICE: return vga_console::green;
	case lib::LogLevel::INFO: return vga_console::bright_green;
	default: panic("Unknown log level");
	}
}

CompositeBuf<log::VgaDelegatingLogBuf, log::SerialDelegatingLogBuf>
build_global_buf(lib::LogLevel level, const char* prefix, lib::LogLevel& enabled_log_level);

}
