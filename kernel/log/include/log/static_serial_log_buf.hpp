#pragma once

#include <streambuf.hpp>
#include <utility.hpp>
#include "simple_stream_buf.hpp"
#include "delegating_log_buf.hpp"
#include "serial_buf.hpp"
#include <static_string.hpp>

namespace log {

template<char ...Prefix>
class StaticSerialLogBuf: public StaticDelegatingLogBuf<Prefix...> {
public:
	StaticSerialLogBuf(lib::LogLevel log_level, const lib::static_string<Prefix...>& prefix, lib::LogLevel& enabled_log_level)
		: StaticDelegatingLogBuf<Prefix...>{log_level, prefix, enabled_log_level, &_delegate},
		_delegate{serial::ComPort::COM1}
	{
		this->delegate(&_delegate);
	}

	StaticSerialLogBuf(StaticSerialLogBuf&& other)
		: StaticDelegatingLogBuf<Prefix...>{lib::move(other)}
		, _delegate{lib::move(other._delegate)}
	{
		this->delegate(&_delegate);
	}

	~StaticSerialLogBuf() {
		this->delegate(nullptr);
	}
private:
	SerialBuf _delegate;
};

}
