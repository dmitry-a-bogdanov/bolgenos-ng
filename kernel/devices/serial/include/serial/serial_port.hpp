#pragma once

#include <cstdint.hpp>

#include <bolgenos-ng/asm.hpp>
#include <mem_utils.hpp>

namespace dev {

enum class ComPort: lib::uint16_t {
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8,
};

struct LineControlRegister {
	LineControlRegister(const uint8_t byte) {
		lib::copy_byte(&byte, this);
	}

	uint8_t _char_size: 2{0};
	bool other: 5{false};
	bool set_baud_rate_divisor: 1{false};

	void set_char_size(uint8_t size) {
		_char_size = size - 5;
	}

	explicit operator uint8_t() const {
		return lib::to_integer<int8_t>(lib::read_byte(this));
	}

	operator lib::byte() const {
		return lib::read_byte(this);
	}
};

static_assert(sizeof(LineControlRegister) == 1);


struct LineStatusRegister {
	LineStatusRegister(const uint8_t byte) {
		lib::copy_byte(&byte, this);
	}

	operator lib::byte() const {
		return lib::read_byte(this);
	}

	bool data_ready: 1{false};
	bool overrun_error: 1{false};
	bool parity_error: 1{false};
	bool framing_error: 1{false};
	bool break_indicator: 1{false};
	bool can_send: 1{false};
	bool transmitter_empty: 1{false};
	bool impending_error: 1{false};
};

struct IOPorts {
	explicit IOPorts(ComPort port): IOPorts(static_cast<uint16_t>(port)) {}
	explicit IOPorts(uint16_t first_address)
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


class SerialPort
{
public:
	SerialPort(ComPort port);

	void set_baud_rate_divisor(uint16_t divisor) {
		LineControlRegister lcr{.set_baud_rate_divisor = true};
		lcr.set_char_size(8);
		outb(_ports.LCR, lcr);
		outb(_ports.DIVISOR_LSB, divisor & 0xff);
		outb(_ports.DIVISOR_MSB, (divisor >> 8) & 0xff);
		lcr.set_baud_rate_divisor = false;
		outb(_ports.LCR, lcr);
	}

	void configure() {
		set_baud_rate_divisor(1);
		for (char c: "Hello!") {
			write(c);
		}
	}

	bool can_write() {
		LineStatusRegister lsr = inb(_ports.LSR);
		return lsr.can_send;
	}


	void write(char chr) {
		while (!can_write()) {}
		outb(_ports.DATA, chr);
	}

private:
	const IOPorts _ports;
};

} // namespace dev
