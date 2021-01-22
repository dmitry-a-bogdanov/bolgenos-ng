#pragma once

#include <serial/serial_port.hpp>
#include <streambuf.hpp>
#include "simple_stream_buf.hpp"
#include "base_log_buf.hpp"
#include "delegating_log_buf.hpp"

class SerialBuf: public SimpleStreamBuf<lib::streambuf>
{
public:
	SerialBuf(serial::SerialPort port);

	int overflow(int c) override;
private:
	serial::SerialPort _port;
};

using SerialLogBuf = BaseLogBuf<SerialBuf>;

namespace log {

class SerialDelegatingLogBuf: public DelegatingLogBuf {
public:
	SerialDelegatingLogBuf(lib::LogLevel log_level, const char* prefix,
			       lib::LogLevel& enabled_log_level, lib::streambuf* delegate);
};

}
