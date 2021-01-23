#include "log/serial_buf.hpp"

#include <utility.hpp>

SerialBuf::SerialBuf(serial::SerialPort port)
	: _port{lib::move(port)}
{}

int SerialBuf::overflow(int c)
{
	if (c == '\n') {
		_port.write('\n');
		_port.write('\r');
	} else {
		_port.write(c);
	}
	return c;
}

log::SerialDelegatingLogBuf::SerialDelegatingLogBuf(lib::LogLevel log_level, const char* prefix,
						    lib::LogLevel& enabled_log_level,
						    lib::streambuf* delegate):
	DelegatingLogBuf(log_level, prefix, enabled_log_level, delegate) {}
