#pragma once

#include <cstddef.hpp>
#include <cstdint.hpp>
#include <mem_utils.hpp>

namespace serial {

struct LineControlRegister {
	LineControlRegister() = default;

	LineControlRegister(const uint8_t byte) {
		lib::copy_byte(&byte, this);
	}

	void set_char_size(uint8_t size) {
		_char_size = size - 5;
	}

	void set_set_baud_rate_divisor(bool set = true) {
		_set_baud_rate_divisor = set;
	}

	void clear_set_baud_rate_divisor() {
		set_set_baud_rate_divisor(false);
	}

	explicit operator uint8_t() const {
		return lib::to_integer<int8_t>(lib::read_byte(this));
	}

	operator lib::byte() const {
		return lib::read_byte(this);
	}

private:
	uint8_t _char_size: 2{0};
	bool _other: 5{false};
	bool _set_baud_rate_divisor: 1{false};
};


}
