#include <serial/io_ports.hpp>

using namespace lib;

serial::IOPorts::IOPorts(uint16_t first_address)
	: DATA(first_address)
	, ENABLE_INTERRUPT(first_address + 1)
	, DIVISOR_LSB(first_address)
	, DIVISOR_MSB(first_address + 1)
	, INTERRUPT_ID(first_address + 2)
	, LCR(first_address + 3)
	, MCR(first_address + 4)
	, LSR(first_address + 5)
	, MSR(first_address + 6)
	, SCRATCH(7)
{}