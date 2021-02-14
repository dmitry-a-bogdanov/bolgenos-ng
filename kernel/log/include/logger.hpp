#pragma once

#include <ostream.hpp>
#include <memory.hpp>

#include "log_level.hpp"
#include "log/composite_buf.hpp"
#include "log/serial_buf.hpp"
#include "log/static_serial_log_buf.hpp"

namespace lib {

template<class Char, Char ...Chars>
class StaticLogger;

template<class Char, Char ...Chars>
class StaticLogger<lib::basic_static_string<Char, Chars...>> {
	using prefix = lib::basic_static_string<Char, Chars...>;
public:
	explicit StaticLogger(const prefix&, LogLevel level) :
		_log_level{level},
		_streambufs{_log_level}
	{}
	constexpr ~StaticLogger() = default;
	constexpr lib::ostream& debug() const { return _debug; }
	constexpr lib::ostream& info() const { return _info; }
	constexpr lib::ostream& notice() const { return _notice; }
	constexpr lib::ostream& warning() const { return _warning; }
	constexpr lib::ostream& error() const { return _error; }
	constexpr lib::ostream& critical() const { return _critical; }

private:
	lib::LogLevel _log_level;

	using serial_sb = log::StaticSerialLogBuf<prefix>;
	using vga_sb = log::StaticVgaDelegatingLogBuf<prefix>;
	using streambuf_type = CompositeBuf<vga_sb, serial_sb>;
	constexpr static auto _prefix = prefix{};

	static streambuf_type build_sb(lib::LogLevel level, lib::LogLevel& conf) {
		return {vga_sb{level, _prefix, conf, log::color(level)}, serial_sb{level, _prefix, conf}};
	}

	struct SbHolder {
		SbHolder(lib::LogLevel& conf)
			: debug{build_sb(LogLevel::DEBUG, conf)}
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

template<class Char, Char ...Prefix> StaticLogger(const lib::basic_static_string<Char, Prefix...>&, LogLevel) -> StaticLogger<Char, Prefix...>;

}

#define LOCAL_LOGGER(prefix, level) static ::lib::StaticLogger<decltype(prefix ## _ss + ": "_ss)> local_logger{prefix ## _ss + ": "_ss, level}

#define LOG_DEBUG local_logger.debug()
#define LOG_INFO local_logger.info()
#define LOG_NOTICE local_logger.notice()
#define LOG_WARN local_logger.warning()
#define LOG_ERROR local_logger.error()
#define LOG_CRIT local_logger.critical()
