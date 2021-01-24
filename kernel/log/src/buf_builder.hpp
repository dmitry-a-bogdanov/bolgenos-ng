#pragma once

#include <log/vga_log_buf.hpp>
#include <log/serial_buf.hpp>
#include <log/composite_buf.hpp>
#include <log.hpp>

namespace log {

CompositeBuf<log::VgaDelegatingLogBuf, log::SerialDelegatingLogBuf>
build_global_buf(lib::LogLevel level, const char* prefix, lib::LogLevel& enabled_log_level);

}
