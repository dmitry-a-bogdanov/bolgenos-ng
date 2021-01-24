#pragma once

#include <ostream.hpp>
#include <memory.hpp>

#include "log_level.hpp"
#include "log/composite_buf.hpp"
#include "log/vga_log_buf.hpp"
#include "log/serial_buf.hpp"
#include "log/static_serial_log_buf.hpp"

namespace lib {

template<char ...Prefix>
class StaticLogger {
public:
	explicit StaticLogger(const lib::static_string<Prefix...>&, LogLevel level) :
		_log_level{level},
		_streambufs{_log_level}
	{}
	~StaticLogger() = default;
	lib::ostream& info() const { return _info; }
	lib::ostream& notice() const { return _notice; }
	lib::ostream& warning() const { return _warning; }
	lib::ostream& error() const { return _error; }
	lib::ostream& critical() const { return _critical; }

private:
	lib::LogLevel _log_level;

	using serial_sb = log::StaticSerialLogBuf<Prefix...>;
	using vga_sb = log::StaticVgaDelegatingLogBuf<Prefix...>;
	using streambuf_type = CompositeBuf<vga_sb, serial_sb>;
	constexpr static auto _prefix = lib::static_string<Prefix...>{};

	static streambuf_type build_sb(lib::LogLevel level, lib::LogLevel& conf) {
		return {vga_sb{level, _prefix, conf, log::color(level)}, serial_sb{level, _prefix, conf}};
	}

	struct SbHolder {
		SbHolder(lib::LogLevel& conf)
			: debug{build_sb(LogLevel::INFO, conf)}
			, info{build_sb(LogLevel::INFO, conf)}
			, notice{build_sb(LogLevel::NOTICE, conf)}
			, warning{build_sb(LogLevel::WARNING, conf)}
			, error{build_sb(LogLevel::ERROR, conf)}
			, critical{build_sb(LogLevel::CRITICAL, conf)}
		{}
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

#define LOCAL_LOGGER(prefix, level) static ::lib::StaticLogger local_logger{prefix ## _ss + ": "_ss, level}

#define LOG_INFO local_logger.info()
#define LOG_NOTICE local_logger.notice()
#define LOG_WARN local_logger.warning()
#define LOG_ERROR local_logger.error()
#define LOG_CRIT local_logger.crical()
