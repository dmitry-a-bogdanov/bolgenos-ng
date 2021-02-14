#pragma once

#include <cstdint.hpp>
#include <mem_utils.hpp>

namespace serial {

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

} // namespace serial
