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

	using streambuf_type = CompositeBuf<log::VgaDelegatingLogBuf, log::StaticSerialLogBuf<Prefix...>>;
	constexpr static auto _prefix = lib::static_string<Prefix...>{};

	struct SbHolder {
		SbHolder(lib::LogLevel& conf)
			: debug{log::StaticSerialLogBuf{LogLevel::INFO, _prefix, conf}}
			, info{log::StaticSerialLogBuf{LogLevel::INFO, _prefix, conf}}
			, notice{log::StaticSerialLogBuf{LogLevel::NOTICE, _prefix, conf}}
			, warning{log::StaticSerialLogBuf{LogLevel::WARNING, _prefix, conf}}
			, error{log::StaticSerialLogBuf{LogLevel::ERROR, _prefix, conf}}
			, critical{log::StaticSerialLogBuf{LogLevel::CRITICAL, _prefix, conf}}
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
