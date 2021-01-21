#include "serial_buf.hpp"

#include <utility.hpp>

SerialBuf::SerialBuf(dev::SerialPort port)
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
