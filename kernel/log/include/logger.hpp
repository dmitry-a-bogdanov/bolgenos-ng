#pragma once

#include <ostream.hpp>
#include <memory.hpp>

#include "log_level.hpp"
#include "log/composite_buf.hpp"
#include "log/vga_log_buf.hpp"
#include "log/serial_buf.hpp"

namespace lib {

class Logger {
public:
	explicit Logger(const char* prefix, LogLevel level);
	~Logger();
	lib::ostream& info() const { return _info; }
	lib::ostream& notice() const { return _notice; }
	lib::ostream& warning() const { return _warning; }
	lib::ostream& error() const { return _error; }
	lib::ostream& critical() const { return _critical; }

private:
	lib::LogLevel _log_level;

	using streambuf_type = CompositeBuf<log::VgaDelegatingLogBuf, log::SerialDelegatingLogBuf>;

	struct SbHolder {
		SbHolder(const char* prefix, lib::LogLevel log_level);
		streambuf_type debug{};
		streambuf_type info{};
		streambuf_type notice{};
		streambuf_type warning{};
		streambuf_type error{};
		streambuf_type critical{};
	} _streambufs;

	mutable lib::ostream _debug{&_streambufs.debug};
	mutable lib::ostream _info{&_streambufs.info};
	mutable lib::ostream _notice{&_streambufs.notice};
	mutable lib::ostream _warning{&_streambufs.warning};
	mutable lib::ostream _error{&_streambufs.error};
	mutable lib::ostream _critical{&_streambufs.critical};
};

}

#define LOCAL_LOGGER(prefix, level) static ::lib::Logger local_logger{prefix, level}

#define LOG_INFO local_logger.info()
#define LOG_NOTICE local_logger.notice()
#define LOG_WARN local_logger.warning()
#define LOG_ERROR local_logger.error()
#define LOG_CRIT local_logger.crical()
