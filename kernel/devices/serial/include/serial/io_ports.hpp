#pragma once

#include <cstdint.hpp>

#include "com_ports.hpp"
#include <bolgenos-ng/asm.hpp>

namespace serial {

struct IOPorts {
	template<x86::IOPort PortType>
	explicit IOPorts(PortType port): IOPorts(static_cast<uint16_t>(port)) {}

	explicit IOPorts(uint16_t first_address);
	IOPorts() = delete;

	const uint16_t DATA;
	const uint16_t ENABLE_INTERRUPT;
	const uint16_t DIVISOR_LSB;
	const uint16_t DIVISOR_MSB;
	const uint16_t INTERRUPT_ID;
	const uint16_t LCR;
	const uint16_t MCR;
	const uint16_t LSR;
	const uint16_t MSR;
	const uint16_t SCRATCH;
};

}
