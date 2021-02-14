#pragma once

#include <cstdint.hpp>

namespace serial {

enum class ComPort: lib::uint16_t {
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8,
};

}
