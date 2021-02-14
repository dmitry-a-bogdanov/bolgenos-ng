#pragma once

#include <bolgenos-ng/asm.hpp>

#include "com_ports.hpp"
#include "io_ports.hpp"
#include "lsr.hpp"

namespace serial {

class SerialPort
{
public:
	SerialPort(ComPort port);

	void set_baud_rate_divisor(uint16_t divisor);

	void configure();

	bool can_write() {
		LineStatusRegister lsr = ::inb(_ports.LSR);
		return lsr.can_send;
	}

	void write(char chr);

private:
	const IOPorts _ports;
};

} // namespace serial
