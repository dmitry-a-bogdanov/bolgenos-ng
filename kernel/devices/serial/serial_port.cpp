#include "serial_port.hpp"


dev::SerialPort::SerialPort(ComPort port): _ports{port} {}

void dev::SerialPort::set_baud_rate_divisor(uint16_t divisor)
{
	LineControlRegister lcr{.set_baud_rate_divisor = true};
	lcr.set_char_size(8);
	outb(_ports.LCR, lcr);
	outb(_ports.DIVISOR_LSB, divisor & 0xff);
	outb(_ports.DIVISOR_MSB, (divisor >> 8) & 0xff);
	lcr.set_baud_rate_divisor = false;
	outb(_ports.LCR, lcr);
}

void dev::SerialPort::configure()
{
	set_baud_rate_divisor(1);
	for (char c: "Hello!") {
		write(c);
	}
}

void dev::SerialPort::write(char chr)
{
	while (!can_write()) {}
	outb(_ports.DATA, chr);
}
