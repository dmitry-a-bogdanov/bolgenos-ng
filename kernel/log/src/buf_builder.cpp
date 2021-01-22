#include "buf_builder.hpp"

#include "streambufs.hpp"

CompositeBuf<log::VgaDelegatingLogBuf, log::SerialDelegatingLogBuf>
log::build_global_buf(lib::LogLevel level, const char* prefix, lib::LogLevel& enabled_log_level)
{
	return {
		log::VgaDelegatingLogBuf{level, prefix, enabled_log_level, &log::sbuf_vga_console, color(level)},
		log::SerialDelegatingLogBuf{level, prefix, enabled_log_level, &log::sbuf_serial_console}
	};
}
