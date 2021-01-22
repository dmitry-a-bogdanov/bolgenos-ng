#include "serial_port.hpp"

#include <serial/lcr.hpp>

serial::SerialPort::SerialPort(ComPort port): _ports{port} {}

void serial::SerialPort::set_baud_rate_divisor(uint16_t divisor)
{
	LineControlRegister lcr;
	lcr.set_set_baud_rate_divisor();
	lcr.set_char_size(8);
	outb(_ports.LCR, lcr);
	outb(_ports.DIVISOR_LSB, divisor & 0xff);
	outb(_ports.DIVISOR_MSB, (divisor >> 8) & 0xff);
	lcr.clear_set_baud_rate_divisor();
	outb(_ports.LCR, lcr);
}

void serial::SerialPort::configure()
{
	set_baud_rate_divisor(1);
	for (char c: "Hello!") {
		write(c);
	}
}

void serial::SerialPort::write(char chr)
{
	while (!can_write()) {}
	outb(_ports.DATA, chr);
}
